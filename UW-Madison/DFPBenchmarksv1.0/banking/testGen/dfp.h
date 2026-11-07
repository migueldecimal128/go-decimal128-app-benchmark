#ifndef DFP_H
#define DFP_H

#ifdef HW
typedef union {
  double a;
  decimal64 b;
} dec64_t;
//end of HW
#else
#define dec64_t dec_t
#endif //!HW

#ifdef BID
#define context_t _IDEC_round
#else
#define context_t decContext
#endif
#endif //DFP_H
