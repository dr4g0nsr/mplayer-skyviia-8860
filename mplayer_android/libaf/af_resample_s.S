
@=========struct af_resample_t===============
.set    u4_step, 		0x001c
.set    u4_pt, 		0x0020
.set    s4_init, 		0x0028
.set    a_in16tmp, 		0x002c
.set    p_w, 		0x0000
.set    p_xq, 		0x0004
.set    u4_xi, 		0x0008
.set    u4_wi, 		0x000c
.set    u4_i, 		0x0010


@=========struct af_data_t===============
.set    p_audio, 		0x0000
.set    u4_len, 		0x0004
.set    s4_nch, 		0x000c


   .text
	.align	2 
	.globl   linint_core
	.globl   polyphae_resample_up
	.globl   polyphae_resample_dn 

linint_core:
   stmfd		sp!,{r4-r11,lr}
   sub      sp, sp, #12
   str      r0, [sp]
   str      r2, [sp, #4]       
   ldr      r4, [r1, #s4_nch]
   ldr      r5, [r2, #u4_step]
   ldr      r0, [r0] @ in16
   ldr      r1, [r1] @ out16 
   ldr      r6, [r2, #u4_pt]
   ldr      r7, [r2, #s4_init]
   mov      r8, #0 @ len
   str      r4, [sp, #8]       
   cmp      r4, #1
   bne      linint_core_L2

linint_core_L1:
   cmp      r7, #0
   bne      linint_core_L1_3
   mov      r7, #1
   str      r7, [r2, #s4_init]
   ldr      r11, =4095
   cmp      r6, r3
   bge      linint_core_L1_2 
linint_core_L1_1:
   mov      r9, r6, asr #12 @ tmp
   and      r10, r6, r11 @ frac
   mov      r9, r9, lsl #1
   add      r9, r0, r9
   ldrsh    lr, [r9]
   ldrsh    r9, [r9, #2]
   add      r8, r8, #1
   add      r6, r6, r5
   sub      r9, r9, lr
   mul      r7, r9, r10
   cmp      r6, r3
   mov      r7, r7, asr #12
   add      r7, r7, lr
   strh     r7, [r1], #2
   blt      linint_core_L1_1
   b        linint_core_L1_8
linint_core_L1_2:
@         }
@         else
@         {
linint_core_L1_3:
   ldr      r11, =4095
   cmp      r6, #4096
   bge      linint_core_L1_5 
@            while(pt < (1<<STEPACCURACY)){
linint_core_L1_4:
   and      r10, r6, r11 @ frac
   ldrsh    r9, [r0]
   ldrsh    lr, [r2, #a_in16tmp]
   add      r8, r8, #1
   add      r6, r6, r5
   sub      r9, r9, lr
   mul      r7, r9, r10
   cmp      r6, #4096
   mov      r7, r7, asr #12
   add      r7, r7, lr
   strh     r7, [r1], #2
   blt      linint_core_L1_4
@            }
linint_core_L1_5:
   add      r3, r3, #4096
   cmp      r6, r3
   bge      linint_core_L1_7
@            while(pt < (end+(1<<STEPACCURACY))){
linint_core_L1_6:
   mov      r9, r6, asr #12 @ tmp
   and      r10, r6, r11 @ frac
   mov      r9, r9, lsl #1
   add      r9, r0, r9
   ldrsh    lr, [r9, #-2]
   ldrsh    r9, [r9]
   add      r8, r8, #1
   add      r6, r6, r5
   sub      r9, r9, lr
   mul      r7, r9, r10
   cmp      r6, r3
   mov      r7, r7, asr #12
   add      r7, r7, lr
   strh     r7, [r1], #2
   blt      linint_core_L1_6
@            }
linint_core_L1_7:
@         }
linint_core_L1_8:
   ldr      r3, [sp] @ c
   and      r6, r6, r11   
   ldr      r4, [r3, #u4_len] @ len
   sub      r4, r4, #2
   mov      r4, r4, asr #1
   mov      r4, r4, lsl #1
   ldrsh    r4, [r0, r4]
   str      r6, [r2, #u4_pt]
   strh     r4, [r2, #a_in16tmp]
   b        linint_core_L3  
linint_core_L2:
   cmp      r7, #0          
   bne      linint_core_L2_4
   mov      r7, #1
   str      r7, [r2, #s4_init]
   ldr      r11, =4095
   ldr      r4, [sp, #8]    
   cmp      r6, r3
   bge      linint_core_L2_3 
linint_core_L2_1:
   mov      r9, r6, asr #12 @ tmp
   and      r10, r6, r11 @ frac
   mul      lr, r9, r4
   mov      r9, lr, lsl #1
   add      r9, r0, r9
   add      r7, r9, r4, lsl #1     
@               for(i=0; i<nch; i++)
linint_core_L2_2:
   ldrsh    lr, [r9], #2
   ldrsh    r12, [r7], #2
   subs     r4, r4, #1
   sub      r12, r12, lr
   mul      r2, r12, r10
   mov      r2, r2, asr #12
   add      r2, r2, lr
   strh     r2, [r1], #2
   bgt      linint_core_L2_2
     
   ldr      r4, [sp, #8]
   add      r6, r6, r5             
   add      r8, r8, r4 @ len
   cmp      r6, r3
   blt      linint_core_L2_1
@            }
linint_core_L2_3:
   b        linint_core_L2_10
@         }
@         else
@         {
linint_core_L2_4:

   ldr      r11, =4095
   cmp      r6, #4096
   ldr      r4, [sp, #8]
  
   bge      linint_core_L2_7 
linint_core_L2_5:
   ldr      r9, [sp, #4]     
   and      r10, r6, r11 @ frac 
   add      r9, r9, #a_in16tmp    
   mov      r7, r0
@               for(i=0; i<nch; i++)
linint_core_L2_6:
   ldrsh    lr, [r9], #2
   ldrsh    r12, [r7], #2
   subs     r4, r4, #1
   sub      r12, r12, lr
   mul      r2, r12, r10
   mov      r2, r2, asr #12
   add      r2, r2, lr
   strh     r2, [r1], #2
   bgt      linint_core_L2_6
        
   ldr      r4, [sp, #8] @ nch
   add      r6, r6, r5             
   add      r8, r8, r4 @ len

   cmp      r6, #4096
   blt      linint_core_L2_5    
linint_core_L2_7:
   add      r3, r3, #4096
   cmp      r6, r3
   ldr      r4, [sp, #8]    
   bge      linint_core_L2_10
@            while(pt < (end+(1<<STEPACCURACY))){
linint_core_L2_8:

   mov      r9, r6, asr #12 @ tmp
   and      r10, r6, r11 @ frac
   mul      lr, r9, r4
   mov      r9, lr, lsl #1
   add      r9, r0, r9
   sub      r7, r9, r4, lsl #1   
linint_core_L2_9:
   ldrsh    lr, [r9], #2
   ldrsh    r12, [r7], #2
   subs     r4, r4, #1
   sub      lr, lr, r12
   mul      r2, lr, r10
   mov      r2, r2, asr #12
   add      r2, r2, r12
   strh     r2, [r1], #2
   bgt      linint_core_L2_9

   ldr      r4, [sp, #8] @ nch 
   add      r6, r6, r5           
   add      r8, r8, r4 @ len

   cmp      r6, r3
   blt      linint_core_L2_8
@            }
linint_core_L2_10:
@         }
   ldr      r4, [sp, #8] @ nch
   ldr      r1, [sp, #4] @ s
   ldr      r3, [sp] @ c
   ldr      r9, [r3, #u4_len] @ len
   add      r12, r1, #a_in16tmp      
   mov      r10, r4, lsl #1
   sub      r9, r9, r10
   mov      r9, r9, asr #1
   mov      r9, r9, lsl #1
   add      r0, r0, r9   
@         for(i=0; i<nch; i++)
linint_core_L2_11:
   ldrsh    lr, [r0], #2
   subs     r4, r4, #1
   strh     lr, [r12], #2
   bgt      linint_core_L2_11
   and      r6, r6, r11
   str      r6, [r1, #u4_pt]

linint_core_L3:
   mov      r0, r8
   add      sp, sp, #12   
   ldmfd    sp!, {r4-r11,pc}
@}
 
@.define ORDERSIXTEEN 

.ifdef ORDERSIXTEEN
.set filter_len_order, 4
.set filter_len_order_shift_1, 5
.set filter_len_order_offset, (1<<filter_len_order_shift_1)
.set filter_len_minus_one, (1<<filter_len_order) - 1 
.else
.set filter_len_order, 3
.set filter_len_order_shift_1, 4
.set filter_len_order_offset, (1<<filter_len_order_shift_1)
.set filter_len_minus_one, (1<<filter_len_order) - 1 
.endif
 
.set sp_len, 24
.set sp_len1, 36  
.set level, sp_len + sp_len1
.set inc, (level+4)
.set nch, (inc+4)
.set up, (nch+4)
.set dn, (up+4)
.set ci, (dn+4)
.set ns, (ci+4)
 
polyphae_resample_up:
   stmfd		sp!,{r4-r11,lr}
   sub      sp, sp, #sp_len   
   ldr      r9, [sp, #nch]   

   stmia    sp, {r0-r3}
   mov      r9, r9, lsl #1
   mov      lr, #0
 
   str      lr, [sp, #16] @ len  
@   while(ci--){
polyphae_resample_up_L1:
   ldr      r4, [sp, #ci]   
   ldr      r7, [sp, #ns]
   sub      r4, r4, #1
   str      r4, [sp, #ci]    
   ldmia    sp, {r0-r3}
   ldr      r8, [r2, #p_xq]   
   ldr      r0, [r0, #p_audio]
   ldr      r1, [r1, #p_audio]
   add      r0, r0, r4, lsl #1 @ in
   add      r8, r8, r4, lsl #2
   ldr      r8, [r8] @ x
   add      r1, r1, r4, lsl #1 @ out 
   add      r7, r0, r7, lsl #1 @ end 
   ldr      r5, [r2, #u4_xi]
   ldr      r6, [r2, #u4_wi]
   ldr      r2, [r2, #u4_i]   
   cmp      r0, r7
   bge      polyphae_resample_up_L6
@      while(in < end){
polyphae_resample_up_L2:

   ldr      r4, [sp, #inc]
   ldr      r12, [sp, #level]
   mov      r2, r4
   cmp      r6, r12 
   addlt    r2, r2, #1
   ldrsh    lr, [r0], r9 @ *(in)
   add      r10, r8, r5, lsl #1 @  x[xi]
   sub      r5, r5, #1
   strh     lr, [r10]
   strh     lr, [r10, #filter_len_order_offset]
   and      r5, r5, #filter_len_minus_one
   cmp      r2, #0 
   ble      polyphae_resample_up_L5
@         while(i--){
polyphae_resample_up_L3:
   sub      r2, r2, #1
   str      r2, [sp, #20]
   add      r10, r8, r5, lsl #1 @  x[xi]
   add      r11, r3, r6, lsl #filter_len_order_shift_1 @ wp
   ldrsh    r12, [r10]
   ldr      lr,  [r11]
   ldrsh    r2, [r10, #2]   
   smulbb   r4, r12, lr
   ldrsh    r12, [r10, #4]  
   smlabt   r4, r2, lr, r4
   ldr      lr,  [r11, #4]   
   ldrsh    r2, [r10, #6]   
   smlabb   r4, r12, lr, r4
   ldrsh    r12, [r10, #8]   
   smlabt   r4, r2, lr, r4
   
   ldr      lr,  [r11, #8]   
   ldrsh    r2, [r10, #10]    
   smlabb   r4, r12, lr, r4
   ldrsh    r12, [r10, #12]   
   smlabt   r4, r2, lr, r4
   
   ldr      lr,  [r11, #12]   
   ldrsh    r2, [r10, #14]
   smlabb   r4, r12, lr, r4
.ifdef ORDERSIXTEEN
   ldrsh    r12, [r10, #16]
.endif   
   smlabt   r4, r2, lr, r4
.ifdef ORDERSIXTEEN
   ldr      lr,  [r11, #16]   
   ldrsh    r2, [r10, #18]
   smlabb   r4, r12, lr, r4
   ldrsh    r12, [r10, #20]   
   smlabt   r4, r2, lr, r4
   
   ldr      lr,  [r11, #20]   
   ldrsh    r2, [r10, #22]      
   smlabb   r4, r12, lr, r4
   ldrsh    r12, [r10, #24]   
   smlabt   r4, r2, lr, r4 
   
   ldr      lr,  [r11, #24]   
   ldrsh    r2, [r10, #26]      
   smlabb   r4, r12, lr, r4
   ldrsh    r12, [r10, #28]   
   smlabt   r4, r2, lr, r4 
   
   ldr      lr,  [r11, #28]   
   ldrsh    r2, [r10, #30]
   smlabb   r4, r12, lr, r4
   smlabt   r4, r2, lr, r4   
            
.endif   
   ldr      r2, [sp, #20]
   ldr      r12, [sp, #16]
   mov      r4, r4, asr #15
   add      r12, r12, #1
   str      r12, [sp, #16]
   ldr      r12, [sp, #dn]
   cmp      r4, #0x8000 
   movge    r4, #0x7f00
   addge    r4, r4, #0xff
   cmnlt    r4, #0x8000
   mvnlt    r4, #0x7f00   
   eorlt    r4, r4, #0xff
   strh     r4, [r1], r9   
   
   ldr      lr, [sp, #up]
   add      r6, r6 ,r12
   cmp      r6, lr
   blt      polyphae_resample_up_L4_1
@            while(wi>=up){
polyphae_resample_up_L4:
@               wi -= up;
   sub      r6, r6, lr
   cmp      r6, lr
   bge      polyphae_resample_dn_L4  
@            };
polyphae_resample_up_L4_1:
   cmp      r2, #0
   bgt      polyphae_resample_up_L3
@         }
polyphae_resample_up_L5:
   cmp      r0, r7
   blt      polyphae_resample_up_L2
@      }
polyphae_resample_up_L6:
   ldr      r4, [sp, #ci]    
   cmp      r4, #0
   bgt      polyphae_resample_up_L1
@   }
   ldr      r1, [sp, #8]
   ldr      r0, [sp, #16]
   str      r6, [r1, #u4_wi]
   str      r5, [r1, #u4_xi]
   add      sp, sp, #sp_len
   ldmfd    sp!, {r4-r11,pc}
@}


polyphae_resample_dn:
   stmfd		sp!,{r4-r11,lr}
   sub      sp, sp, #sp_len   
   ldr      r9, [sp, #nch]   

   stmia    sp, {r0-r3}
   mov      r9, r9, lsl #1
   mov      lr, #0
 
   str      lr, [sp, #16] @ len          
@   while(ci--){
polyphae_resample_dn_L1:
   ldr      r4, [sp, #ci]   
   ldr      r7, [sp, #ns]
   sub      r4, r4, #1
   str      r4, [sp, #ci]    
   ldmia    sp, {r0-r3}
   ldr      r8, [r2, #p_xq]   
   ldr      r0, [r0, #p_audio]
   ldr      r1, [r1, #p_audio]
   add      r0, r0, r4, lsl #1 @ in
   add      r8, r8, r4, lsl #2
   ldr      r8, [r8] @ x
   add      r1, r1, r4, lsl #1 @ out 
   add      r7, r0, r7, lsl #1 @ end 
   ldr      r5, [r2, #u4_xi]
   ldr      r6, [r2, #u4_wi]
   ldr      r2, [r2, #u4_i]   
   cmp      r0, r7
   bge      polyphae_resample_dn_L6   
@      while(in < end){
polyphae_resample_dn_L2:
   ldrsh    lr, [r0], r9 @ *(in)
   add      r10, r8, r5, lsl #1 @  x[xi]
   sub      r5, r5, #1
   strh     lr, [r10]
   strh     lr, [r10, #filter_len_order_offset]
   and      r5, r5, #filter_len_minus_one
   sub      r2, r2, #1
   cmp      r2, #0 
   bgt      polyphae_resample_dn_L5
   str      r2, [sp, #20]
   add      r10, r8, r5, lsl #1 @  x[xi]
   add      r11, r3, r6, lsl #filter_len_order_shift_1 @ wp
   ldrsh    r12, [r10]
   ldr      lr,  [r11]
   ldrsh    r2, [r10, #2]   
   smulbb   r4, r12, lr
   ldrsh    r12, [r10, #4]  
   smlabt   r4, r2, lr, r4
   ldr      lr,  [r11, #4]   
   ldrsh    r2, [r10, #6]   
   smlabb   r4, r12, lr, r4
   ldrsh    r12, [r10, #8]   
   smlabt   r4, r2, lr, r4
   
   ldr      lr,  [r11, #8]   
   ldrsh    r2, [r10, #10]    
   smlabb   r4, r12, lr, r4
   ldrsh    r12, [r10, #12]   
   smlabt   r4, r2, lr, r4
   
   ldr      lr,  [r11, #12]   
   ldrsh    r2, [r10, #14]
   smlabb   r4, r12, lr, r4
.ifdef ORDERSIXTEEN
   ldrsh    r12, [r10, #16]
.endif   
   smlabt   r4, r2, lr, r4
.ifdef ORDERSIXTEEN
   ldr      lr,  [r11, #16]   
   ldrsh    r2, [r10, #18]
   smlabb   r4, r12, lr, r4
   ldrsh    r12, [r10, #20]   
   smlabt   r4, r2, lr, r4
   
   ldr      lr,  [r11, #20]   
   ldrsh    r2, [r10, #22]      
   smlabb   r4, r12, lr, r4
   ldrsh    r12, [r10, #24]   
   smlabt   r4, r2, lr, r4 
   
   ldr      lr,  [r11, #24]   
   ldrsh    r2, [r10, #26]      
   smlabb   r4, r12, lr, r4
   ldrsh    r12, [r10, #28]   
   smlabt   r4, r2, lr, r4 
   
   ldr      lr,  [r11, #28]   
   ldrsh    r2, [r10, #30]
   smlabb   r4, r12, lr, r4
   smlabt   r4, r2, lr, r4   
            
.endif                 
   ldr      r2, [sp, #20]
   ldr      r12, [sp, #16]
   mov      r4, r4, asr #15
   add      r12, r12, #1
   str      r12, [sp, #16]
   ldr      r12, [sp, #dn]
   cmp      r4, #0x8000 
   movge    r4, #0x7f00
   addge    r4, r4, #0xff
   cmnlt    r4, #0x8000
   mvnlt    r4, #0x7f00   
   eorlt    r4, r4, #0xff
   strh     r4, [r1], r9
   ldr      lr, [sp, #up]
   add      r6, r6 ,r12
   cmp      r6, lr
   blt      polyphae_resample_dn_L4
@            while(wi>=up){
polyphae_resample_dn_L3:
@               wi -= up;
   sub      r6, r6, lr
   cmp      r6, lr
   bge      polyphae_resample_dn_L3   
@            };
polyphae_resample_dn_L4:
   ldr      r4, [sp, #inc]
   ldr      r12, [sp, #level]
   mov      r2, r4
   cmp      r6, r12   
   addlt    r2, r2, #1
@         }
polyphae_resample_dn_L5:
   cmp      r0, r7
   blt      polyphae_resample_dn_L2
@      }
polyphae_resample_dn_L6:
   ldr      r4, [sp, #ci]    
   cmp      r4, #0
   bgt      polyphae_resample_dn_L1
   ldr      r1, [sp, #8]
   ldr      r0, [sp, #16]
   str      r6, [r1, #u4_wi]
   str      r5, [r1, #u4_xi]
   str      r2, [r1, #u4_i]       
@   return len;
   add      sp, sp, #sp_len
   ldmfd    sp!, {r4-r11,pc}
@}
