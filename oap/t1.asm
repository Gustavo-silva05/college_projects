 #SQRT - T1
 # T0 = i
 # T1 = x
  
.data
print_1: .asciiz "Programa de Raiz Quadrada Newton-Raphson \nDesenvolvedores: <Gustavo Souza>\n"
print_x_i: .asciiz "\nDigite os parâmetros x e i para calcular sqrt_nr (x, i) ou -1 para abortar a execução: "
print_r1: .asciiz "sqrt("
print_r2: .asciiz ") = "
print_r3: .asciiz ", "
x: .space 4
i: .space 4

.text
.global intro

intro:	la $a0, print_1		#
	li $v0, 4		# cout << print_1;
	syscall			#
	
main:	la $a0, print_x_i	#
	li $v0, 4		# cout << print_x;
	syscall			#	
	li $v0, 5		# cin >> x;
	syscall			#
	move $t1, $v0		#
	bltz $t1, fim		# if (x < 0) negativo;
	
	li $v0, 5		# cin >> i;
	syscall			#
	move $t0, $v0		#
	bltz $t0, fim		# if (i < 0) negativo;
	
	la $a0, print_r1	#
	li $v0, 4		# cout << print_r1;	
	syscall			#
	
	move $a0, $t1		#
	li $v0, 1		# cout << x;
	syscall			#
	
	la $a0, print_r3	#
	li $v0, 4		# cout << print_r3;
	syscall			#
	
	move $a0, $t0		#
	li $v0, 1		# cout << i;
	syscall			#
	
	la $a0, print_r2	#
	li $v0, 4		# cout << print_r2;
	syscall			#
	
	jal sqrt_nr		# sqrt(x,i);
	
	addi $sp, $sp, 4	# pilha.size() -= 1;
	
	move $a0, $v0		#
	li $v0, 1		# cout << v0;  (resultado final) 
	syscall			#
	
	j main			# return 0; (encerra)

sqrt_nr:	addi $sp, $sp, -4	# pilha.size() += 1;
		sw $ra, 0($sp)		# pilha[size] = $ra;
		bgtz $t0, recursao	# if (i > 0) recursao;
		li $v0, 1		# v0 = 1;
		jr $ra			# return $ra
		
recursao:	subi $t0, $t0, 1	# i --;
		jal sqrt_nr		# sqrt (x, i-1);
		div $t1, $v0		# LO = x/1;
		mflo $t2		# t2 = LO;
		add $v0, $v0 , $t2	# v0 = v0 + t2;
		srl $v0, $v0, 1		# v0 /= 2;
		addi $sp, $sp, 4	# pilha.size() -= 1;
		lw $ra, 0($sp) 		# $ra = pilha[size];
		jr $ra			# return $ra;
		
fim: 	li $v0, 10	# return 0; (encerramento);
	syscall		#
