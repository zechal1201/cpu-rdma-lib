#ifndef CPU_RDMA_H
#define CPU_RDMA_H

#include <mpi.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========== Result Codes ========== */
typedef enum {
    ACCEL_SUCCESS = 0,
    ACCEL_ERROR_INVALID_PARAM = -1,
    ACCEL_ERROR_NO_DEVICE = -2,
    ACCEL_ERROR_MEMORY_REG = -3,
    ACCEL_ERROR_CONNECTION = -4,
    ACCEL_ERROR_SEND_FAILED = -5,
    ACCEL_ERROR_RECV_FAILED = -6,
    ACCEL_ERROR_ALLTOALL_FAILED = -7,
    ACCEL_ERROR_TIMEOUT = -8,
    ACCEL_ERROR_MPI_FAILED = -9,
    ACCEL_ERROR_COMM_NOT_CREATED = -10,
} acclResult_t;

/* ========== Data Types ========== */
typedef enum {
    ACCEL_DTYPE_BYTE = 1,
    ACCEL_DTYPE_INT = 4,
    ACCEL_DTYPE_LONG = 8,
    ACCEL_DTYPE_FLOAT = 5,
    ACCEL_DTYPE_DOUBLE = 6,
} acclDataType_t;

/* ========== Handles ========== */
typedef struct acclComm* acclComm_t;

typedef struct {
    uint64_t request_id;
    int state;
} acclRequest_t;

/* ========== Data Structures ========== */
typedef struct {
    int rank;
    int size;
    uint32_t context_id;
    char comm_name[256];
} acclCommInfo_t;

typedef struct {
    int global_rank;
    uint32_t qp_num;
    uint32_t lid;
    uint8_t gid[16];
    uint32_t psn;
    int rdma_device_id;
} acclRankInfo_t;

typedef struct {
    int rank;
    uint32_t qp_num;
    uint32_t lid;
    uint8_t gid[16];
    uint32_t psn;
    uint64_t remote_mem_addr;
    uint32_t remote_mem_key;
} acclPeerInfo_t;

typedef struct {
    int num_peers;
    acclPeerInfo_t* peers;
} acclVCT_t;

typedef enum {
    ACCL_SUM, ACCL_PROD, ACCL_MIN, ACCL_MAX,
    ACCL_BAND, ACCL_BOR, ACCL_BXOR,
} acclOp_t;

/* ========== Communicator Creation ========== */
acclResult_t acclCommCreateFromMPI(MPI_Comm mpi_comm, uint16_t rdma_port, acclComm_t* comm);
acclResult_t acclCommDup(acclComm_t src_comm, acclComm_t* dest_comm);
acclResult_t acclCommSplit(acclComm_t comm, int color, int key, acclComm_t* new_comm);
acclResult_t acclCommGetInfo(acclComm_t comm, acclCommInfo_t* info);
acclResult_t acclCommGetVCT(acclComm_t comm, acclVCT_t* vct);
acclResult_t acclCommFreeVCT(acclVCT_t* vct);
acclResult_t acclCommFree(acclComm_t* comm);

/* ========== Synchronization ========== */
acclResult_t acclBarrier(acclComm_t comm);
acclResult_t acclBcast(acclComm_t comm, void* buffer, size_t count, acclDataType_t datatype, int root);

/* ========== Point-to-Point Operations ========== */
acclResult_t acclSendSync(acclComm_t comm, const void* sendbuff, size_t count, acclDataType_t datatype, int dest_rank);
acclResult_t acclRecvSync(acclComm_t comm, void* recvbuff, size_t count, acclDataType_t datatype, int src_rank);
acclResult_t acclSendAsync(acclComm_t comm, const void* sendbuff, size_t count, acclDataType_t datatype, int dest_rank, acclRequest_t* request);
acclResult_t acclRecvAsync(acclComm_t comm, void* recvbuff, size_t count, acclDataType_t datatype, int src_rank, acclRequest_t* request);
acclResult_t acclSendrecv(acclComm_t comm, const void* sendbuff, size_t sendcount, acclDataType_t sendtype, int dest_rank, void* recvbuff, size_t recvcount, acclDataType_t recvtype, int src_rank);
acclResult_t acclWaitRequest(acclComm_t comm, acclRequest_t* request, int timeout_ms);

/* ========== Collective Operations ========== */
acclResult_t acclAllToAll(acclComm_t comm, const void *sendbuff, size_t sendcount, acclDataType_t sendtype, void *recvbuff, size_t recvcount, acclDataType_t recvtype);
acclResult_t acclAllToAllv(acclComm_t comm, const void *sendbuff, const size_t sendcounts[], const size_t sdispls[], acclDataType_t sendtype, void *recvbuff, const size_t recvcounts[], const size_t rdispls[], acclDataType_t recvtype);
acclResult_t acclAllToAllm(acclComm_t comm, const void *sendbuff, const size_t *sendcountmatrix, acclDataType_t sendtype, void *recvbuff, acclDataType_t recvtype);
acclResult_t acclGather(acclComm_t comm, const void *sendbuff, size_t sendcount, acclDataType_t sendtype, void *recvbuff, size_t recvcount, acclDataType_t recvtype, int root);
acclResult_t acclScatter(acclComm_t comm, const void *sendbuff, size_t sendcount, acclDataType_t sendtype, void *recvbuff, size_t recvcount, acclDataType_t recvtype, int root);
acclResult_t acclReduce(acclComm_t comm, const void *sendbuff, void *recvbuff, size_t count, acclDataType_t datatype, acclOp_t op, int root);
acclResult_t acclAllReduce(acclComm_t comm, const void *sendbuff, void *recvbuff, size_t count, acclDataType_t datatype, acclOp_t op);

/* ========== Diagnostics ========== */
acclResult_t acclCommGetStats(acclComm_t comm, char* stats_buffer, size_t buffer_size);
acclResult_t acclCommResetStats(acclComm_t comm);
acclResult_t acclCommEnableTracing(acclComm_t comm, const char* trace_file);
const char* acclGetLastErrorMsg(void);

#ifdef __cplusplus
}
#endif

#endif /* CPU_RDMA_H */
