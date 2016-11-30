#ifndef NULLMQ_H_
#define NULLMQ_H_

#include <vector>
#include <string>
#include <stdint.h>

class nullMQ {
protected:
	struct socket_internal {
		std::string *name;
		void *data;
		size_t msg_size;
		uint32_t id;
		bool isRead;
		pthread_rwlock_t lock;
		pthread_cond_t cond;
		pthread_mutex_t cond_mutex;
		pthread_cond_t read_cond;
		pthread_mutex_t read_cond_mutex;
	};
	static std::vector<socket_internal*> sockets;
	static pthread_mutex_t connect_mutex;

public:
	class socket_t {
		friend class nullMQ;
	protected:
		uint32_t lastRead;

	public:
		socket_internal *ptr;
		/**
		 * sends data
		 *
		 * The size of the data is determined when creating the socket
		 *
		 * @param data
		 *   pointer to the data to be sent
		 * @param nonblock
		 *   whether the call should wait until the last data has been read
		 */
		void send(void *data, bool nonblock = true);

		/**
		 * receives data
		 *
		 * @param buffer
		 *   preallocated buffer where the received data is written. If no
		 *   data is received, buffer contents remain unchanged.
		 * @param nonblock
		 *   whether the call should wait for new data (false, default) or
		 *   return if there is no new data available (true)
		 *
		 * @return
		 *   wether new data has been received and the contents of buffer
		 *   have been changed
		 */
		bool recv(void *buffer, bool nonblock = false);

	};

	/**
	 * connects to a socket
	 *
	 * @param socket_name
	 *   name of the socket
	 * @param msg_size
	 *   size of the messages transmitted through the socket
	 *
	 * @return
	 *   the requested socket
	 */
	static socket_t connect(const std::string &socket_name, size_t msg_size);

};

#endif /* NULLMQ_H_ */
