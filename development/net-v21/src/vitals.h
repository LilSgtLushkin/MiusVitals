// Tactical Vitals, integrated directly into net_v21; no legacy v20 DLL hooks.
#pragma once
#include <stdint.h>
struct ObjMan;
void vitals_start();
void vitals_pause();
uint32_t vitals_epoch();
void vitals_peer(uint32_t epoch);
void vitals_on_packet(const unsigned char *data, int size, void (*send)(const void *, int));
void vitals_frame(ObjMan *om, uint32_t now, void (*send)(const void *, int));
#ifdef NETV21_TEST_HOOKS
void vitals_test_mock_layout(bool enabled);
uint32_t vitals_test_peer_epoch();
unsigned vitals_test_outstanding();
uint64_t vitals_test_received();
void vitals_test_stat_invoke(void *object, void (*original)(void *, long long, char));
#endif
