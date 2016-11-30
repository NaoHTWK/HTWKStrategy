#include "nullmq.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <limits>
#include <stdexcept>
#include <string>

std::vector<nullMQ::socket_internal*> nullMQ::sockets=std::vector<nullMQ::socket_internal*>();
pthread_mutex_t nullMQ::connect_mutex=PTHREAD_MUTEX_INITIALIZER;

nullMQ::socket_t nullMQ::connect(const std::string &_socket_name, size_t _msg_size){
	if(_socket_name.empty()||_msg_size==0)
        throw(std::invalid_argument("Socket name can't be empty and message size can't be null!"));

	pthread_mutex_lock(&connect_mutex);
    for(const auto& s : sockets) {
        if(_socket_name==*(s->name)){

            if(s->msg_size != _msg_size) {
                printf("nullMQ::connect: Message size isn't the same: orginal %zu != argument %zu\n.", s->msg_size, _msg_size);
		        exit(1);
		    }

			socket_t r;
            r.ptr=s;
			r.lastRead=0;
			pthread_mutex_unlock(&connect_mutex);
			return r;
		}
	}
	socket_internal *s=(socket_internal*)malloc(sizeof(socket_internal));
	pthread_cond_init(&(s->cond),NULL);
	pthread_mutex_init(&(s->cond_mutex),NULL);
	pthread_cond_init(&(s->read_cond),NULL);
	pthread_mutex_init(&(s->read_cond_mutex),NULL);
	s->data=(void*)malloc(_msg_size);
	s->id=0;
	s->isRead=true;
	pthread_rwlock_init(&(s->lock),NULL);
	s->msg_size=_msg_size;
	s->name=new std::string(_socket_name);
	sockets.push_back(s);
	socket_t r;
	r.ptr=s;
	r.lastRead=0;
	pthread_mutex_unlock(&connect_mutex);
	return r;
}


void nullMQ::socket_t::send(void *_data, bool _nonblock){
	if(_data==NULL)
        throw(std::invalid_argument("nullMQ::send Data can't be a null pointer!"));
    pthread_rwlock_wrlock(&(ptr->lock));
	while(!_nonblock&&!ptr->isRead){
		pthread_mutex_lock(&(ptr->read_cond_mutex));
		pthread_rwlock_unlock(&(ptr->lock));
		pthread_cond_wait(&(ptr->read_cond),&(ptr->read_cond_mutex));
		pthread_mutex_unlock(&(ptr->read_cond_mutex));
		pthread_rwlock_wrlock(&(ptr->lock));
	}
	memcpy(ptr->data,_data,ptr->msg_size);
	ptr->id=(ptr->id==std::numeric_limits<uint32_t>::max())?1:((ptr->id)+1);
	ptr->isRead=false;
	pthread_mutex_lock(&(ptr->cond_mutex));
	pthread_cond_broadcast(&(ptr->cond));
	pthread_mutex_unlock(&(ptr->cond_mutex));
	pthread_rwlock_unlock(&(ptr->lock));
}


bool nullMQ::socket_t::recv(void *_buffer, bool _nonblock){
	if(_buffer==NULL)
        throw(std::invalid_argument("nullMQ::recv Buffer can't be null!"));
    if(ptr->id==lastRead){
		if(_nonblock){
			return false;
		}else{
			pthread_rwlock_rdlock(&(ptr->lock));
			if(ptr->id!=lastRead){
				memcpy(_buffer,ptr->data,ptr->msg_size);
				lastRead=ptr->id;
				ptr->isRead=true;
				pthread_mutex_lock(&(ptr->read_cond_mutex));
				pthread_cond_broadcast(&(ptr->read_cond));
				pthread_mutex_unlock(&(ptr->read_cond_mutex));
				pthread_rwlock_unlock(&(ptr->lock));
				return true;
			}else{
				pthread_mutex_lock(&(ptr->cond_mutex));
				pthread_rwlock_unlock(&(ptr->lock));
				pthread_cond_wait(&(ptr->cond),&(ptr->cond_mutex));
				pthread_mutex_unlock(&(ptr->cond_mutex));
			}
		}
	}
	pthread_rwlock_rdlock(&(ptr->lock));
	memcpy(_buffer,ptr->data,ptr->msg_size);
	lastRead=ptr->id;
	ptr->isRead=true;
	pthread_mutex_lock(&(ptr->read_cond_mutex));
	pthread_cond_broadcast(&(ptr->read_cond));
	pthread_mutex_unlock(&(ptr->read_cond_mutex));
	pthread_rwlock_unlock(&(ptr->lock));
	return true;
}


