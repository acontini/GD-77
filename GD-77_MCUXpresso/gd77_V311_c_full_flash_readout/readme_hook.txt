USB communication hook:

...
      0x39568: 0xf7ff 0xff5f  BL        0x3942a
      0x3956c: 0x2800         CMP       R0, #0
      0x3956e: 0xf040 0x80fd  BNE.W     0x3976c
------------------------- => patch to jump out to hook
      0x39572: 0xf89d 0x000c  LDRB.W    R0, [SP, #0xc]
      0x39576: 0x284d         CMP       R0, #77                 ; 0x4d
      0x39578: 0xf040 0x80f8  BNE.W     0x3976c
      0x3957c: 0xf89d 0x000d  LDRB.W    R0, [SP, #0xd]
      0x39580: 0x2802         CMP       R0, #2
      0x39582: 0xf040 0x80f3  BNE.W     0x3976c
      0x39586: 0xf7e9 0xfe85  BL        0x23294
------------------------- <= come back to 0x3958a from hook if above is through in hook OR jump back from hook to 0x3976c
      0x3958a: 0x2110         MOVS      R1, #16                 ; 0x10
      0x3958c: 0xf20f 0x2020  ADR.W     R0, . + 0x224           ; 0x397b0
      0x39590: 0xf7e9 0xfe7d  BL        0x2328e
...
