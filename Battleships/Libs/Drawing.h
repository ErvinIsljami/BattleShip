#pragma once
#ifndef   DRAWING_H
#define   DRAWING_H

#define   CP437     437
#define   CP850     850
#define   UTF8      8

#ifndef   CHARSET
#ifdef    _WIN32
#define   CHARSET   CP437
#else
#define   CHARSET   UTF8
#endif
#endif

#if (CHARSET == CP437 || CHARSET == CP850)

/* Windows code page 437 box drawing characters */
#define  BOX_DLR      "\315"  /* ═ */
#define  BOX_DUD      "\272"  /* ║ */
#define  BOX_DUL      "\274"  /* ╝ */
#define  BOX_DUR      "\310"  /* ╚ */
#define  BOX_DDL      "\273"  /* ╗ */
#define  BOX_DDR      "\311"  /* ╔ */
#define  BOX_DUDL     "\271"  /* ╣ */
#define  BOX_DUDR     "\314"  /* ╠ */
#define  BOX_DULR     "\312"  /* ╩ */
#define  BOX_DDLR     "\313"  /* ╦ */
#define  BOX_DUDLR    "\316"  /* ╬ */
#define  BOX_DU_SL    "\275"  /* ╜, not in CP850 */
#define  BOX_DU_SR    "\323"  /* ╙, not in CP850 */
#define  BOX_DD_SL    "\267"  /* ╖, not in CP850 */
#define  BOX_DD_SR    "\326"  /* ╓, not in CP850 */
#define  BOX_DL_SU    "\276"  /* ╛, not in CP850 */
#define  BOX_DL_SD    "\270"  /* ╕, not in CP850 */
#define  BOX_DR_SU    "\324"  /* ╘, not in CP850 */
#define  BOX_DR_SD    "\325"  /* ╒, not in CP850 */
#define  BOX_DU_SLR   "\320"  /* ╨, not in CP850 */
#define  BOX_DD_SLR   "\322"  /* ╥, not in CP850 */
#define  BOX_DL_SUD   "\265"  /* ╡, not in CP850 */
#define  BOX_DR_SUD   "\306"  /* ╞, not in CP850 */
#define  BOX_DLR_SU   "\317"  /* ╧, not in CP850 */
#define  BOX_DLR_SD   "\321"  /* ╤, not in CP850 */
#define  BOX_DLR_SUD  "\330"  /* ╪, not in CP850 */
#define  BOX_DUD_SL   "\266"  /* ╢, not in CP850 */
#define  BOX_DUD_SR   "\307"  /* ╟, not in CP850 */
#define  BOX_DUD_SLR  "\327"  /* ╫, not in CP850 */
#define  BOX_SLR      "\304"  /* ─ */
#define  BOX_SUD      "\263"  /* │ */
#define  BOX_SUL      "\331"  /* ┘ */
#define  BOX_SUR      "\300"  /* └ */
#define  BOX_SDL      "\277"  /* ┐ */
#define  BOX_SDR      "\332"  /* ┌ */
#define  BOX_SULR     "\301"  /* ┴ */
#define  BOX_SDLR     "\302"  /* ┬ */
#define  BOX_SUDL     "\264"  /* ┤ */
#define  BOX_SUDR     "\303"  /* ├ */
#define  BOX_SUDLR    "\305"  /* ┼ */

#elif CHARSET == UTF8

/* UTF-8 box drawing characters */
#define  BOX_DLR      "\342\225\220"  /* ═ */
#define  BOX_DUD      "\342\225\221"  /* ║ */
#define  BOX_DUL      "\342\225\235"  /* ╝ */
#define  BOX_DUR      "\342\225\232"  /* ╚ */
#define  BOX_DDL      "\342\225\227"  /* ╗ */
#define  BOX_DDR      "\342\225\224"  /* ╔ */
#define  BOX_DUDL     "\342\225\243"  /* ╣ */
#define  BOX_DUDR     "\342\225\240"  /* ╠ */
#define  BOX_DULR     "\342\225\251"  /* ╩ */
#define  BOX_DDLR     "\342\225\246"  /* ╦ */
#define  BOX_DUDLR    "\342\225\254"  /* ╬ */
#define  BOX_DU_SL    "\342\225\234"  /* ╜ */
#define  BOX_DU_SR    "\342\225\231"  /* ╙ */
#define  BOX_DD_SL    "\342\225\226"  /* ╖ */
#define  BOX_DD_SR    "\342\225\223"  /* ╓ */
#define  BOX_DL_SU    "\342\225\233"  /* ╛ */
#define  BOX_DL_SD    "\342\225\225"  /* ╕ */
#define  BOX_DR_SU    "\342\225\230"  /* ╘ */
#define  BOX_DR_SD    "\342\225\222"  /* ╒ */
#define  BOX_DU_SLR   "\342\225\250"  /* ╨ */
#define  BOX_DD_SLR   "\342\225\245"  /* ╥ */
#define  BOX_DL_SUD   "\342\225\241"  /* ╡ */
#define  BOX_DR_SUD   "\342\225\236"  /* ╞ */
#define  BOX_DLR_SU   "\342\225\247"  /* ╧ */
#define  BOX_DLR_SD   "\342\225\244"  /* ╤ */
#define  BOX_DLR_SUD  "\342\225\252"  /* ╪ */
#define  BOX_DUD_SL   "\342\225\242"  /* ╢ */
#define  BOX_DUD_SR   "\342\225\237"  /* ╟ */
#define  BOX_DUD_SLR  "\342\225\253"  /* ╫ */
#define  BOX_SLR      "\342\224\200"  /* ─ */
#define  BOX_SUD      "\342\224\202"  /* │ */
#define  BOX_SUL      "\342\224\230"  /* ┘ */
#define  BOX_SUR      "\342\224\224"  /* └ */
#define  BOX_SDL      "\342\224\220"  /* ┐ */
#define  BOX_SDR      "\342\224\214"  /* ┌ */
#define  BOX_SULR     "\342\224\264"  /* ┴ */
#define  BOX_SDLR     "\342\224\254"  /* ┬ */
#define  BOX_SUDL     "\342\224\244"  /* ┤ */
#define  BOX_SUDR     "\342\224\234"  /* ├ */
#define  BOX_SUDLR    "\342\224\274"  /* ┼ */

#else
#error : Box drawing characters are not defined for this charset.
""
#endif
#endif /* DRAWING_H */

void draw_top_side();
void draw_one_row(LIST *head, int row);
void draw_bottom_side(LIST *head, int row);
void draw_table(LIST *head);

