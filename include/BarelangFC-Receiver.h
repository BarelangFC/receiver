#ifndef BARELANGFC_RECEIVER_H_
#define BARELANGFC_RECEIVER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT_ 5002
#define BUFLEN_ 4096 //Max length of buffer

namespace BarelangFC {
        class BarelangReceiver {
                private:
			char * parse;
			char recv[BUFLEN_];
			int index_;

			struct sockaddr_in si_meV, si_otherV, addrTerima, addrKirim;
			socklen_t slenV, slen = sizeof(addrKirim);
			int v, recv_lenV, socTrim;

                public:
			int roll, pitch, yaw, voltage, walkActive, supportLeg, lKneeCurr, rKneeCurr, vx, vy, va;

			void initialize_();
			void processing_();
			void die(char *s);
        };
}
#endif
