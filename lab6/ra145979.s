.globl troca_endianness_imprime              @torna a visibilidade da função imprime global - útil para que outros módulos possam chamar tal função
.extern printf


.text
troca_endianness_imprime:
  
  push {r4, lr}
  mov r4, r0                @endereço do vetor é passado via r0; coloca o endereço em r4  
  mov r3, #0                @zera r3, que será usado como variável de indução do laço

laco:
  add r2, r4, r3, lsl #2    @cômputo do *endereço* do i-ésimo elemento do vetor
@note que na instrução acima, lsl #2 provoca um deslocamento de 2 bits em r3, multiplicando seu valor por 4 - 4 bytes é justamente o tamanho de um inteiro.
  
  ldr r1, [r2]              @carrega o *valor* do i-ésimo elemento do vetor em r1
  ldr r0, =print_mask       @carrega a máscara de printf em r0
  
  push {r3}                 @salva o valor de r3 para que printf não estrague
  bl printf
  pop {r3}
  
  add r3, r3, #1            @incremento de r3
  
  cmp r3, #10
  blo laco                  @se (r3 < 10) salta para o rótulo laco
  
  pop {r4, pc}              @ volta para instrucao após chamada de imprime

converte_para_little_endian: @pega o valor armazenado em r1 e converte para little_endian
  push{r2}
  ldb r2, r1 @Coloca em r2 o byte menos significativo de r1
  mov r1, r1, lsr #1        @
  mov r2, r2, lsl #1        @
  pop{r2}


.data
  print_mask: .asciz "%u\n" @máscara do printf
