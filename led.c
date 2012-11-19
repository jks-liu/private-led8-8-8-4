#include "msp430.h"
#include "font.h"

/* P14, CP: shift register clock  */
#define CP 0x10
#define SHCP_SET() P1OUT |= CP
#define SHCP_RESET() P1OUT &= ~CP
/* P15, ST: storage register clock */
#define ST 0x20
#define STCP_SET() P1OUT |= ST
#define STCP_RESET() P1OUT &= ~ST
/* P24, R1: upper partion*/
#define R1 0x10
#define UPPER_SET() P2OUT |= R1
#define UPPER_RESET() P2OUT &= ~R1
/* P25, R2: lower partion*/
#define R2 0x20
#define LOWER_SET() P2OUT |= R2
#define LOWER_RESET() P2OUT &= ~R2
/* P20 - P23 */
#define CHOOSE_LINE(x) P2OUT = (P2OUT & ~0x0f) | (x)

static volatile unsigned char g_table_upper[128];
static volatile unsigned char g_table_lower[128];

typedef struct {
  unsigned char flag;
  unsigned char x;
  unsigned char y;
  unsigned char ok;
  unsigned char clear;
} Recived_xy;

static volatile Recived_xy g_received_xy;

void clear_display (void) 
{
  for (unsigned char i = 0; i < 128; ++ i) {
    g_table_upper[i] = 0;
    g_table_lower[i] = 0;
  }
  
  return;
}

int main (void)
{
  WDTCTL = WDTPW + WDTHOLD;             
  DCOCTL = 0; 
  BCSCTL1 = CALBC1_12MHZ;                   // Set DCO
  DCOCTL = CALDCO_12MHZ;
  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 226;                            // 1MHz 9600
  UCA0BR1 = 4;                              // 1MHz 9600
  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

  __bis_SR_register(GIE);       // Enter LPM0, interrupts enabled
  
  P1DIR |= 0x30; 
  P2DIR |= 0x3f;
    
  g_table_lower[8] = 0x99;
  g_table_upper[2] = 0x01;

  for (;;)
  {
#ifdef DISPLAY_HANZI
    for (unsigned char i = 0; i < 16; ++i) {
      for (unsigned char j = 0; j < 4; ++j) {
        for (signed char k = 15; (k < 16) && (k >= 0) ; --k) {
          if (0 != ((table_dalianligong[(i << 1) + (j << 5) + !(k >> 3)]) & (0x01 << (k & 0x07)))) {
            UPPER_RESET();
          } else {
            UPPER_SET();
          }
          if (0 != ((ASCII8x8_Table[(('5' - 0x20) << 0x03) + (i & 0x07)]) & (0x01 << (k & 0x07)))) {
            LOWER_RESET();
          } else {
            LOWER_SET();
          }
          
          SHCP_RESET();
          SHCP_SET();
        } /* End for k*/
      } /*End for j */
        
        CHOOSE_LINE(i);
        STCP_RESET(); 
        STCP_SET();
    } /* End for i*/
#endif /* DISPLAY_HANZI */
    
    for (unsigned char line = 0; line < 16 ; ++line) {
      for (unsigned char short_line = 0; short_line < 8; ++ short_line) {
        for (unsigned char column = 0; column < 8; ++column) {
          if (0 == (g_table_upper[(line << 3) + short_line] & (0x01 << (7 - column)))) {
            UPPER_SET();
          } else {
            UPPER_RESET();
          }
          
          if (0 == (g_table_lower[(line << 3) + short_line] & (0x01 << (7 - column)))) {
            LOWER_SET();
          } else {
            LOWER_RESET();
          }
          
          SHCP_RESET();
          SHCP_SET();
        } /* End for column */
      } /* End for short_line */
            
      CHOOSE_LINE(line);
      STCP_RESET();
      STCP_SET();
      
      if (g_received_xy.ok == 1)
      {
        g_received_xy.ok = 0;
        
        unsigned char fix_x = g_received_xy.x;
        unsigned char fix_y = g_received_xy.y;
        
        if (fix_y < 16) {
          g_table_upper[(fix_x >> 3) + (fix_y << 3)] |= 
            0x01 << (7 - (fix_x % 8));
        } else {
          g_table_lower[(fix_x >> 3) + ((fix_y - 16) << 3)] |= 
            0x01 << (7 - (fix_x % 8));
        }
      } /* End if */
      
      if (g_received_xy.clear == 1) {
        g_received_xy.clear = 0;
        clear_display();
      }
    } /* End for line */
  } /* End ifinite loop*/
}



//  Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{                 
  static unsigned char recived_byte;
  recived_byte = UCA0RXBUF;
  while (!(IFG2&UCA0TXIFG));     
  UCA0TXBUF = recived_byte;  
  
  if (g_received_xy.ok == 1) return;
  
  switch (recived_byte) {
  case 'x': 
    g_received_xy.flag = 'x';
    break;
  case 'y':
    g_received_xy.flag = 'y';
    break;
  case 'c':
    g_received_xy.clear = 1;
    break;
  default:
    if (g_received_xy.flag == 'x') {
      g_received_xy.x = recived_byte;
    } else {
      g_received_xy.y = recived_byte;
      g_received_xy.ok = 1;
    }
    
    break;
  }
  
  return;
}