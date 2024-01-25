
// Conventional types and constants for Frobio

#define true (bool)1
#define false (bool)0
#define OKAY (errnum)0
#define NOTYET (errnum)1

typedef unsigned char bool;
typedef unsigned char byte;
typedef unsigned char errnum;
typedef unsigned int word;
typedef void (*func_t)();

#define Poke(A,B) (*(volatile byte*)(A) = (byte)(B))
#define Poke2(A,W) (*(volatile word*)(A) = (word)(W))

#define Peek(A) (*(volatile byte*)(A))
#define Peek2(A) (*(volatile word*)(A))

#if 1

#define TCP_CHUNK_SIZE 1024  // Chunk to send or recv in TCP.
#define WIZ_PORT  0xFF68   // Hardware port.
#define WAITER_TCP_PORT  2319   // w# a# i#
#define CASBUF 0x01DA // Rock the CASBUF, rock the CASBUF!
#define VDG_RAM  0x0400  // default 32x16 64-char screen
#define VDG_END  0x0600
#define PACKET_BUF 0x600
#define PACKET_MAX 256

#include "frob3/wiz/w5100s_defs.h"

// REQUIRES GCC FOR 6809:
#include <stdarg.h>
typedef unsigned int size_t;
#define restrict
void abort(void);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);
void *memset(void *s, int c, size_t n);
char *strcpy(char *restrict dest, const char *restrict src);
char *strncpy(char *restrict dest, const char *restrict src, size_t n);
size_t strlen(const char *s);
#undef restrict


// How to talk to the four hardware ports.
struct wiz_port {
  volatile byte command;
  volatile word addr;
  volatile byte data;
};

#define WIZ  ((struct wiz_port*)0xFF68)

// Constants for the (four) Wiznet sockets' registers.
struct sock {
    word base;
    word tx_ring;
    word rx_ring;
};
extern const struct sock WizSocketFacts[4];

#define SOCK0_AND (Socks+0),
#define JUST_SOCK0 (Socks+0)

#define SOCK1_AND (Socks+1),
#define JUST_SOCK1 (Socks+1)

#define PARAM_JUST_SOCK   const struct sock* sockp
#define PARAM_SOCK_AND    const struct sock* sockp,
#define JUST_SOCK         sockp
#define SOCK_AND          sockp,

#define B    (sockp->base)
#define T    (sockp->tx_ring)
#define R    (sockp->rx_ring)
#define N    (sockp->nth)

#define RING_SIZE 2048
#define RING_MASK (RING_SIZE - 1)

struct proto {
  byte open_mode;
  byte open_status;
  bool is_broadcast;
  byte send_command;
};
extern const struct proto TcpProto;
extern const struct proto UdpProto;
extern const struct proto BroadcastUdpProto;

extern const byte HexAlphabet[];
byte IsThisGomar();
byte WizGet1(word reg);
word WizGet2(word reg);
void WizGetN(word reg, void* buffer, word size);
void WizPut1(word reg, byte value);
void WizPut2(word reg, word value);
void WizPutN(word reg, const void* data, word size);
word WizTicks();
byte WizTocks();
void WizReset();
void WizConfigure();
void WizIssueCommand(PARAM_SOCK_AND byte cmd);
void WizWaitStatus(PARAM_SOCK_AND byte want);

typedef word tx_ptr_t;

void WizOpen(PARAM_SOCK_AND const struct proto* proto, word local_port );
void TcpDial(PARAM_SOCK_AND const byte* host, word port);
void TcpEstablish(PARAM_JUST_SOCK);
errnum WizCheck(PARAM_JUST_SOCK);
errnum WizRecvGetBytesWaiting(PARAM_SOCK_AND word* bytes_waiting_out);
tx_ptr_t WizReserveToSend(PARAM_SOCK_AND  size_t n);
tx_ptr_t WizDataToSend(PARAM_SOCK_AND tx_ptr_t tx_ptr, const char* data, size_t n);
tx_ptr_t WizBytesToSend(PARAM_SOCK_AND tx_ptr_t tx_ptr, const byte* data, size_t n);
void WizFinalizeSend(PARAM_SOCK_AND const struct proto *proto, size_t n);
errnum WizSendChunk(PARAM_SOCK_AND  const struct proto* proto, char* data, size_t n);
errnum WizRecvChunkTry(PARAM_SOCK_AND char* buf, size_t n);
errnum WizRecvChunk(PARAM_SOCK_AND char* buf, size_t n);
errnum WizRecvChunkBytes(PARAM_SOCK_AND byte* buf, size_t n);
errnum TcpRecv(PARAM_SOCK_AND char* p, size_t n);
errnum TcpSend(PARAM_SOCK_AND  char* p, size_t n);
void UdpDial(PARAM_SOCK_AND  const struct proto *proto,
             const byte* dest_ip, word dest_port);
void WizClose(PARAM_JUST_SOCK);

////////////////////////////////////////////////////////
///
///  GCC Standard Library Routines.

void* memcpy(void* dest, const void* src, size_t n) {
  char* d = (char*)dest;
  char* s = (char*)src;
  for (size_t i = 0; i < n; i++) *d++ = *s++;
  return dest;
}

void *memset(void *s, int c, size_t n) {
  char* dest = (char*)s;
  for (size_t i = 0; i < n; i++) *dest++ = c;
  return s;
}

#if 0
char *strcpy(char *restrict dest, const char *restrict src) {
  void* z = dest;
  while (*src) *dest++ = *src++;
  return z;
}

char *strncpy(char *restrict dest, const char *restrict src, size_t n) {
  void* z = dest;
  int i = 0;
  while (*src) {
    *dest++ = *src++;
    i++;
    if (i>=n) break;
  }
  return z;
}

size_t strlen(const char *s) {
  const char* p = s;
  while (*p) p++;
  return p-s;
}

size_t strnlen(const char *s, size_t max) {
  const char* p = s;
  while (*p && (p-s < max)) p++;
  return p-s;
}
#endif

////////////////////////////////////////////////////////

void HitachiTransferFromPort(word from, word to, word count) {

  asm volatile(
    "  ldw %2\n"
    "  ldx %0\n"
    "  ldy %1\n"
    "  orcc #$50\n"
    "  tfm x,y+\n"
    "  andcc #$AF\n"
    : // no outputs
    : "m" (from), "m" (to), "m" (count) // inputs
    : "x", "y" // clobbered
  );
}



////////////////////////////////////////////////////////

const byte HexAlphabet[] = "0123456789abcdef";

const struct sock Socks[2] = {
    { 0x400, 0x4000, 0x6000 },
    { 0x500, 0x4800, 0x6800 },
    // not needed // { 0x600, 0x5000, 0x7000 },
    // not needed // { 0x700, 0x5800, 0x7800 },
};

const struct proto TcpProto = {
  SK_MR_TCP+SK_MR_ND, // TCP Protocol mode with No Delayed Ack.
  SK_SR_INIT,
  false,
  SK_CR_SEND,
};

void Delay(word n) {
  while (n--) {
#ifdef __GNUC__
    asm volatile ("mul" : : : "d", "b", "a");
    asm volatile ("mul" : : : "d", "b", "a");
    asm volatile ("mul" : : : "d", "b", "a");
    asm volatile ("mul" : : : "d", "b", "a");
    asm volatile ("mul" : : : "d", "b", "a");
#else
    asm {
      mul
      mul
      mul
      mul
      mul
    }
#endif
  }
}


#if 1
void Enable1BitSound(void) {
    *(volatile byte*)0xFF23 &= ~0x04;  // Clear bit 2 to enable Data Direction access
    *(volatile byte*)0xFF22 |= 0x02;   // Bit 1 and bits 3-7 are outputs.
    *(volatile byte*)0xFF23 |= 0x04;  // Clear bit 2 to enable Data Direction access
}

void Pia1bOn(byte x) { *(volatile byte*)0xFF22 |= x; }
void Pia1bOff(byte x) { *(volatile byte*)0xFF22 &= ~x; }
void Orange(void) { Pia1bOn(0x08); }
void Green(void) { Pia1bOff(0x08); }

void Beep(byte n, byte f) {
    Enable1BitSound();
    for (byte i = 0; i < n; i++) {
        Pia1bOn(0x02);
        Delay(f<<2);
        Pia1bOff(0x02);
        Delay(f<<2);
        Pia1bOn(0x02);
        Delay(f);
        Pia1bOff(0x02);
        Delay(f);
    }
}

void Fatal(void) {
  while (1) {
    Beep(50, 12);
    Delay(5000);
  }
}
#endif

union wordbytes {
	byte b[2];
	word w;
};

byte WizGet1(word reg) {
  WIZ->addr = reg;
  return WIZ->data;
}
word WizGet2(word reg) {
#if 0
  WIZ->addr = reg;
  byte z_hi = WIZ->data;
  byte z_lo = WIZ->data;
  return ((word)(z_hi) << 8) + (word)z_lo;
#else
  union wordbytes u;
  WIZ->addr = reg;
  u.b[0] = WIZ->data;
  u.b[1] = WIZ->data;
  return u.w;
#endif
}
void WizGetN(word reg, void* buffer, word size) {
#if 1
  #if 0
  volatile struct wiz_port* wiz = WIZ;
  byte* to = (byte*) buffer;
  wiz->addr = reg;
  for (word i=size; i; i--) {
    *to++ = wiz->data;
  }
  #else
  byte* to = (byte*) buffer;
  WIZ->addr = reg;

  byte octs = (byte)(size>>3);
  byte spare = (byte)size & 7;

  switch (spare) {
    case 7:
      *to++ = WIZ->data; /*FALLTHROUGH*/
    case 6:
      *to++ = WIZ->data; /*FALLTHROUGH*/
    case 5:
      *to++ = WIZ->data; /*FALLTHROUGH*/
    case 4:
      *to++ = WIZ->data; /*FALLTHROUGH*/
    case 3:
      *to++ = WIZ->data; /*FALLTHROUGH*/
    case 2:
      *to++ = WIZ->data; /*FALLTHROUGH*/
    case 1:
      *to++ = WIZ->data; /*FALLTHROUGH*/
    case 0:
      {}
  }

  for (byte i=octs; i; i--) {
    *to++ = WIZ->data;
    *to++ = WIZ->data;
    *to++ = WIZ->data;
    *to++ = WIZ->data;
    *to++ = WIZ->data;
    *to++ = WIZ->data;
    *to++ = WIZ->data;
    *to++ = WIZ->data;
  }
  #endif
#else
  WIZ->addr = reg;
  HitachiTransferFromPort((word)&(WIZ->data), (word)buffer, size);
#endif
}
void WizPut1(word reg, byte value) {
  WIZ->addr = reg;
  WIZ->data = value;
}
void WizPut2(word reg, word value) {
#if 0
  WIZ->addr = reg;
  WIZ->data = (byte)(value >> 8);
  WIZ->data = (byte)(value);
#else
  union wordbytes u;
  u.w = value;
  WIZ->addr = reg;
  WIZ->data = u.b[0];
  WIZ->data = u.b[1];
#endif
}

#if 0
void WizPutN(word reg, const void* data, word size) {
  volatile struct wiz_port* wiz = WIZ;
  const byte* from = (const byte*) data;
  wiz->addr = reg;
  for (word i=size; i; i--) {
    wiz->data = *from++;
  }
}
#endif


void WizIssueCommand(const struct sock* sockp, byte cmd) {
  WizPut1(B+SK_CR, cmd);
#if 0
  while (WizGet1(B+SK_CR)) {
    // ++ *(char*)0x401;
  }
#endif
  // if (cmd == 0x40) PutChar('<');
  // else if (cmd == 0x20) PutChar('>');
  // else PutChar('!');
}

#if 0
void WizWaitStatus(const struct sock* sockp, byte want) {
  byte status;
  byte stuck = 200;
  do {
    // ++ *(char*)0x400;
    status = WizGet1(B+SK_SR);
    if (!--stuck) Fatal();
  } while (status != want);
}
#endif

////////////////////////////////////////////////

// Only called for TCP Client.
// Can probably be skipped.
errnum WizCheck(PARAM_JUST_SOCK) {
      byte ir = WizGet1(B+SK_IR); // Socket Interrupt Register.
      if (ir & SK_IR_TOUT) { // Timeout?
        return SK_IR_TOUT;
      }
      if (ir & SK_IR_DISC) { // Disconnect?
        return SK_IR_DISC;
      }
      return OKAY;
}

errnum WizRecvGetBytesWaiting(const struct sock* sockp, word* bytes_waiting_out) {
  errnum e = WizCheck(JUST_SOCK);
  if (e) return e;

  *bytes_waiting_out = WizGet2(B+SK_RX_RSR0);  // Unread Received Size.
  return OKAY;
}

errnum QuickTcpRecv(const struct sock* sockp, char* buf, size_t n) {
  word bw = WizGet2(B+SK_RX_RSR0);
  if (bw < n) {
  	Poke(0xFF9A, 0x04); // Red border.
  	while (1) DisableInterrupts();
  }

  word rd = WizGet2(B+SK_RX_RD0);
  word begin = rd & RING_MASK; // begin: Beneath RING_SIZE.
  word end = begin + n;    // end: Sum may not be beneath RING_SIZE.

  if (end >= RING_SIZE) {
    word first_n = RING_SIZE - begin;
    word second_n = n - first_n;
    WizGetN(R+begin, buf, first_n);
    WizGetN(R, buf+first_n, second_n);
  } else {
    WizGetN(R+begin, buf, n);
  }

  WizPut2(B+SK_RX_RD0, rd + n);
  WizIssueCommand(SOCK_AND  SK_CR_RECV); // Inform socket of changed SK_RX_RD.
  return OKAY;
}

errnum WizRecvChunkTry(const struct sock* sockp, char* buf, size_t n) {
#if 0
  word bytes_waiting = 0;
  errnum e = WizRecvGetBytesWaiting(SOCK_AND &bytes_waiting);
  if (e) return e;
  if( bytes_waiting < n) return NOTYET;
#endif
  word rd = WizGet2(B+SK_RX_RD0);
  word begin = rd & RING_MASK; // begin: Beneath RING_SIZE.
  word end = begin + n;    // end: Sum may not be beneath RING_SIZE.

  if (end >= RING_SIZE) {
    word first_n = RING_SIZE - begin;
    word second_n = n - first_n;
    WizGetN(R+begin, buf, first_n);
    WizGetN(R, buf+first_n, second_n);
  } else {
    WizGetN(R+begin, buf, n);
  }

  WizPut2(B+SK_RX_RD0, rd + n);
  WizIssueCommand(SOCK_AND  SK_CR_RECV); // Inform socket of changed SK_RX_RD.
  return OKAY;
}

errnum WizRecvChunk(const struct sock* sockp, char* buf, size_t n) {
  // PrintH("WizRecvChunk %x...", n);
  errnum e;
  do {
    e = WizRecvChunkTry(SOCK_AND buf, n);
  } while (e == NOTYET);
  // PrintH("WRC %x: %x %x %x %x %x.", n, buf[0], buf[1], buf[2], buf[3], buf[4]);
  return e;
}
errnum WizRecvChunkBytes(const struct sock* sockp, byte* buf, size_t n) {
  return WizRecvChunk(sockp, (char*)buf, n);
}
errnum TcpRecv(const struct sock* sockp, char* p, size_t n) {
  while (n) {
    word chunk = (n < TCP_CHUNK_SIZE) ? n : TCP_CHUNK_SIZE;
    errnum e = WizRecvChunk(SOCK_AND  p, chunk);
    if (e) return e;
    n -= chunk;
    p += chunk;
  }
  return OKAY;
}
#endif

////////////////////////////////////////////////

void EnableInterrupts(void) {
  asm volatile("andcc #$AF");
}
void DisableInterrupts(void) {
  asm volatile("orcc #$50");
}

void HitachiMode(void) {
  asm volatile("ldmd #1"
      : // no ouputs
      : // no inputs
      : // no clobbers
  );
}

void CallWithNewStack(word new_stack, func_t fn) {
  asm volatile("ldx %0\n  ldy %1\n  tfr x,s\n  jsr ,y"
      : // no outputs
      : "m" (new_stack), "m" (fn) // two inputs
  );
}

void BeepBusySignal(void) {
  Enable1BitSound();
  while (1) {
    Beep(8, 12);
    Delay(2000);
  }
}

#define grafbuf ((byte*)0x3000)

void TwoBeeps(void) {
  Enable1BitSound();
  Beep(10, 8);
  Delay(1000);
  Beep(10, 8);
  Delay(1000);
}

char colors[] = {
	3, 3, 3, // white
	2, 2, 2, // light gray
	1, 1, 1, // gray
	0, 0, 0, // black

	// lighter:
	3, 1, 1, // red
	0, 3, 3, // anti-red

	1, 3, 1, // green
	3, 0, 3, // anti-green

	1, 1, 3, // blue
	3, 3, 0, // anti-blue

	// darker:
	2, 0, 0, // red
	0, 1, 1, // anti-red

	0, 2, 0, // green
	1, 0, 1, // anti-green

	0, 0, 2, // blue
	1, 1, 0, // anti-blue
};

void SetPalette(void) {
  for (byte i=0; i < 16; i++) {
  	byte red = colors[3*i+0];
  	byte green = colors[3*i+1];
  	byte blue = colors[3*i+2];

	byte x = 0;
	if (red&2) x |= 0x20;
	if (green&2) x |= 0x10;
	if (blue&2) x |= 0x08;
	if (red&1) x |= 0x04;
	if (green&1) x |= 0x02;
	if (blue&1) x |= 0x01;
	Poke(0xFFB0+i, x);
  }
}

void SetupHres(byte hres, byte slpr) {
  // slpr: scanlines per row: 0 for 1, 2 for 2.
  /*
  Poke2(0xFFA4, 0x0405); // blocks 4 & 5
  Poke2(0xFFAC, 0x0405); // ... also in task 1
  Poke2(0xFFA4, 0x0607); // blocks 6 & 7
  Poke2(0xFFAC, 0x0607); // ... also in task 1
  */

  Poke(0xFF90, 0x04);  // SCS normal
  // Poke(0xFF91) not now 
  Poke(0xFF92, 0);  // no IRQ
  Poke(0xFF93, 0);  // no FIRQ

  Poke(0xFF98, 0x80 | slpr); // Graphics. 60Hz. slpr=lines/row.
  Poke(0xFF99, 0x02 | (hres<<2)); // 192lines. HRES. Colors=16

  Poke(0xFF9A, 0x05); // Purple border for now
  SetPalette();

  Poke(0xFF9C, 0); // No vertical scroll
  Poke(0xFF9F, 0); // No horz offset

  word hibits = (word)0xE0; // three highest bits.
  Poke(0xFF9D, (hibits | ((word)grafbuf>>(8+3))));  // bits 18-11
  Poke(0xFF9E, ((word)grafbuf>>3));      // bits 10-3

  Poke(0xFFD6, 255); // R0=0 1.78 MHz
  Poke(0xFFD9, 255); // R1=1
}

void IrqHandlerWrapper() {
  asm volatile(
    "  bra END_HNDL ; skip all when using outer function\n"
    "_HANDLER:\n"
    "  clr $FF92 ; clear timer irq\n"
    "  lda #$20  ; bit for timer\n"
    "  sta $FF92 ; re-enable timer irq\n"

    "  ;ldx #$7000\n"
    "  ;inc ,x\n"

    "  ldx <2    ; sound table ptr\n"
    "  ldb ,x+   ; sound sample\n"
    "  andb #$FC ; only top 6 buts\n"
    "  stb $FF20 ; DAC out\n"
    "  cmpx #$7000+3200 ; end?\n"
    "  blo SKIP_HNDL    ; skip if not\n"

    "  ldx #$7000   ; restore to start of table\n"
    "  ;ldy #$3010\n"
    "  ;inc ,y\n"

    "SKIP_HNDL:\n"
    "  stx <2    ; save new ptr\n"


    "  rti\n"
    "END_HNDL:\n"
    "  nop"
    : // no outputs
    : // no inputs
    : // no clobbers
  );
}

extern char HANDLER[];

void SetupDacSound() {
  // Set sound Mux switch to 0, with Data enabled.
  Poke(0xFF01, 0x34);  // LSBit of Mux is $08 bit, set to 0.
  Poke(0xFF03, 0x34);  // MSBit of Mux is $08 bit, set to 0.

  Poke(0xFF21, 0x30); // choose Data Direction (cassette off)
  Poke(0xFF20, 0xff); // all outputs
  Poke(0xFF21, 0x34); // choose Data (cassette off)

  Poke(0xFF23, 0x3C); // Enable Sound! and choose Data

  Poke2(2, 0x7000);  // Init sound table ptr to start of table at $7000.
#if 0
  //xx// Poke2(4, 0x7000+3200);  // Ptr to end of sound table.
  // Initial junk in sound table.
  for (word p= 0x7000; p < 0x7000+3200; p++) {
  	Poke(p, 128 + ((p&16) ? 90 : -90));
  }
#endif

  // request interrupts.
  // IRQ:
  Poke(0x010C, 0x7E);  // $7E == "JMP >EXTENDED"
  Poke2(0x010D, HANDLER);
  // FRQ:
  Poke(0x010F, 0x7E);  // $7E == "JMP >EXTENDED"
  Poke2(0x0110, HANDLER);

  //Poke2(0xFFF6, HANDLER);  // Set FIRQ Handler.
  //Poke2(0xFFF8, HANDLER);  // Set IRQ Handler.

  Poke(0xFF91, 0x60);  // fast clock.   (does bit 6 memory type matter?)
  Poke(0xFF92, 1<<5);  // Enable IRQ on timer.
  Poke(0xFF93, 0);  // Disable all FIRQs.
  // Poke2(0xFF94, 1787); // 14.3 MHz / 8000 Hz = 1787.5
  Poke2(0xFF94, 447); // 3.579546 MHz / 8000 Hz = 1787.5
  Poke(0xFF90, 0x24);  // Enable IRQ; SCS normal
}

#if 0
void RampWave() {
  while (1) {
    for (byte i = 0; i < 64; i++) {
      // Poke(0xFF20, (i<<2));
      if (i&32) {
      	Poke(0xFF20, (63-i)<<2);
      } else {
      	Poke(0xFF20, i<<2);
      }
      Delay(20);
    }
  }
}
#endif

// Pick one group of HRES mode and BPR BytesPerRow

// #define HRES 5
// #define BPR 80

// HRES 4 is nice with 2 scan lines per row.
#define HRES 4
#define BPR 64

// #define HRES 3
// #define BPR 40

// #define HRES 2
// #define BPR 32

void main2(void) {
  DisableInterrupts();
  // HitachiMode();

  byte scan_lines_per_row = 2; // 1 or 2

  IrqHandlerWrapper();
  //RampWave();

  SetupHres(HRES, scan_lines_per_row);
  SetupDacSound();  // After SetupHres.

#if 0
  while (1) {
	 byte* p = (char*)grafbuf;
	 TcpRecv(SOCK1_AND  p, 800);
	 for (word i=0; i<800; i++) {
      		Poke(0xFF20, p[i] & 0xFC);
		Delay(1);
	 }
  }
#else

  // TcpRecv(SOCK1_AND  0x7000, 1600);  // preload audio 1/5 second (at 8000 Hz)
	 {
	   byte* ap = 0x7000;
	   for (byte i = 0; i < 16; i ++) {
           	QuickTcpRecv(SOCK1_AND  ap, 100);  // load audio 1/5 second (at 8000 Hz)
		ap += 100;
	   }
	 }

  EnableInterrupts();

  byte audio_phase = true;
  while (1) {

	 {
	   byte* ap = audio_phase ? 0x7000+1600 : 0x7000;
	   for (byte i = 0; i < 16; i ++) {
           	QuickTcpRecv(SOCK1_AND  ap, 100);  // load audio 1/5 second (at 8000 Hz)
		ap += 100;
	   }
	 }

#if 0

//#define NLINES 1 // was 16
#define NLINES 32
	 char* p = (char*)grafbuf;
	  for (word i=0; i<(192/NLINES/scan_lines_per_row); i++) {
		TcpRecv(SOCK1_AND  p, NLINES*BPR);
		// memcpy(p+0x4000, p, NLINES*BPR);
		p += NLINES*BPR;
	  }

#else
	 char* p = (char*)grafbuf;
	 for (byte i=0; i<(192/scan_lines_per_row); i++) {
		QuickTcpRecv(SOCK1_AND  p, BPR);
		p += BPR;
	  }

#endif

#if 1
	  if (!audio_phase) {
	  	byte* x = 0x3000;
	  	while(Peek2(2) < 0x7000+1600) {
			// *x++ = 0x44;
		}
	  } else {
	  	byte* x = 0x3000;
	  	while(Peek2(2) >= 0x7000+1600) {
			// *x++ = 0x55;
		}
	  }
#endif

	 audio_phase = !audio_phase;
  }
#endif
}

int main() {
    CallWithNewStack(0x0800, &main2);
}
