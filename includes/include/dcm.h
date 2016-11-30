#ifndef DCM_H
#define DCM_H

typedef int naohtwk_timestamp_t;//!<DCM-based timestamp, x*10ms from last naoqi start, 0 is the oldest value
struct dcm_data {
    naohtwk_timestamp_t dcm_time;
};

#define NAO_HTWK_TIMESTAMPS_PER_SEC (100)

#endif // DCM_H
