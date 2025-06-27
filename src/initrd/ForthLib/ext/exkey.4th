
#define EXKEY_UP                         0x480000
#define EXKEY_DOWN                       0x500000
#define EXKEY_RIGHT                      0x4D0000
#define EXKEY_LEFT                       0x4B0000
#define EXKEY_HOME                       0x470000
#define EXKEY_END                        0x4F0000
#define EXKEY_INSERT                     0x520000
#define EXKEY_DELETE                     0x530000
#define EXKEY_PAGE_UP                    0x490000
#define EXKEY_PAGE_DOWN                  0x510000
#define EXKEY_F1                         0x3B0000
#define EXKEY_F2                         0x3C0000
#define EXKEY_F3                         0x3D0000
#define EXKEY_F4                         0x3E0000
#define EXKEY_F5                         0x3F0000
#define EXKEY_F6                         0x400000
#define EXKEY_F7                         0x410000
#define EXKEY_F8                         0x420000
#define EXKEY_F9                         0x430000
#define EXKEY_F10                        0x440000
#define EXKEY_F11                        0x570000
#define EXKEY_F12                        0x580000

\- CTL+		: CTL+   $200 OR ;
\- ALT+		: ALT+   $400 OR ;

VARIABLE CTLALT

VARIABLE LASTKEY 


: EXKEY? ( --flg )
  LASTKEY @ IF -1 BREAK

  SCANKEY? 0=
  IF  0     KEY?1 IF KEY1 CTLALT @ OR LASTKEY ! 1- THEN
  BREAK

  SCANKEY
  DUP KBDUS + C@ IF DROP 0 BREAK
  DUP $1D =	IF DROP $200 CTLALT OR! 0 BREAK		\ Ctrl press
  DUP $9D =	IF DROP $200 INVERT CTLALT AND! 0 BREAK	\ Ctrl release
  DUP $38 =	IF DROP $400 CTLALT OR! 0 BREAK		\ Alt press
  DUP $B8 =	IF DROP $400 INVERT CTLALT AND! 0 BREAK \ Alt release
  DUP $80 AND	IF DROP 0 BREAK

    $10 LSHIFT  CTLALT @ OR LASTKEY ! -1
;

: EXKEY
    CURSORON
 BEGIN EXKEY? UNTIL
 LASTKEY @ LASTKEY OFF
    CURSOROFF
;
: KEYTST1
  BEGIN  SCANKEY DUP H. 
 ." <"  KBDUS + C@ H. ." >" AGAIN
;

: KEYTST2
  BEGIN EXKEY DUP H. $1B = UNTIL
;
: KEYMV
\ dup h.
 DUP EXKEY_UP		= IF DROP GETXY 1- SETXY 0 BREAK
 DUP EXKEY_DOWN		= IF DROP GETXY 1+ SETXY 0 BREAK
 DUP EXKEY_RIGHT	= IF DROP GETXY SWAP 1+ SWAP SETXY 0 BREAK
 DUP EXKEY_LEFT		= IF DROP GETXY SWAP 1- SWAP SETXY 0 BREAK
 -1
;

: KEYTST3
  BEGIN
  begin EXKEY KEYMV until

 DUP H. $1B = UNTIL
;

: KEYTST4
  BEGIN
  begin EXKEY KEYMV until

 DUP emit $1B = UNTIL
;

\ KEYTST4
 ' EXKEY TO KEY

