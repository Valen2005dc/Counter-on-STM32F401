#include "stm32f4xx.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>

int dp[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x00, 0x10};//0x80 <- 0x00
int a = 0;
int b = 0;
int res=0;
int num=0;
int numarr[100];

void EXTI1_IRQHandler(void) {
    if ((EXTI->PR & EXTI_PR_PR1) != 0) { // Check if EXTI line 1 triggered the interrupt
        EXTI->PR |= EXTI_PR_PR1; // Clear the pending interrupt
        if (a < 9) {
            a++; // Increment the counter
        } else {
            a = 0; // Reset the counter if it reaches 9
						if(b<9){
							b++;
						} else {
							b=0;
						}
        }
    }
}

void EXTI2_IRQHandler(void) {
    if ((EXTI->PR & EXTI_PR_PR2) != 0) { // Check if EXTI line 2 triggered the interrupt
        EXTI->PR |= EXTI_PR_PR2; // Clear the pending interrupt
				num+=(b*10+a);
				numarr[res]=b*10+a;
        a = 0; // Reset variable a
        b = 0; // Reset variable b
				res++;
    }
}

void dispNum (int nd, int nu){
		GPIOC->ODR = dp[nu];
		GPIOB->ODR = dp[nd];
}

void spDig(int nu) {
		dispNum(nu/10, nu%10);
}


double med(int arr[], int size) {
    double suma = 0;
    for (int i = 0; i < size; i++) {
        suma += arr[i];
    }
    return suma / size;
}

// Función para calcular la varianza de un array de números
void Varz(int arr[], int size) {
    double media = med(arr, size);
    double suma_cuadrados = 0;
    for (int i = 0; i < size; i++) {
        suma_cuadrados += (arr[i] - media) * (arr[i] - media);
    }
    spDig(suma_cuadrados / size);
}

void calcModa(int arr[], int size) {
    int moda = 0; // Valor de la moda
    int maxCount = 0; // Contador máximo de ocurrencias
    for (int i = 0; i < size; i++) {
        int count = 0; // Contador de ocurrencias del número actual
        for (int j = 0; j < size; j++) {
            if (arr[j] == arr[i]) {
                count++; // Incrementar contador si el número se repite
            }
        }
        if (count > maxCount) {
            maxCount = count; // Actualizar el contador máximo
            moda = arr[i]; // Actualizar el valor de la moda
        }
    }
    spDig(moda); // Devolver la moda
}


int main(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enable GPIOC clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Enable SYSCFG clock

    // Configure PA1 and PA2 as input with pull-up resistors
    GPIOA->MODER &= ~(GPIO_MODER_MODER1 | GPIO_MODER_MODER2); // Input mode for PA1 and PA2
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR1_0 | GPIO_PUPDR_PUPDR2_0; // Pull-up for PA1 and PA2

    // Configure EXTI line 1 for PA1
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI1; // Select PA1 as the source input for EXTI1
    EXTI->IMR |= EXTI_IMR_MR1; // Enable interrupt on EXTI line 1
    EXTI->FTSR |= EXTI_FTSR_TR1; // Set falling trigger

    // Configure EXTI line 2 for PA2
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2; // Select PA2 as the source input for EXTI2
    EXTI->IMR |= EXTI_IMR_MR2; // Enable interrupt on EXTI line 2
    EXTI->FTSR |= EXTI_FTSR_TR2; // Set falling trigger

    NVIC_EnableIRQ(EXTI1_IRQn); // Enable EXTI1 interrupt
    NVIC_SetPriority(EXTI1_IRQn, 0); // Set priority

    NVIC_EnableIRQ(EXTI2_IRQn); // Enable EXTI2 interrupt
    NVIC_SetPriority(EXTI2_IRQn, 0); // Set priority

    // Configure GPIOB and GPIOC for 7-segment display
    GPIOB->MODER |= 0x5555; // Output mode for GPIOB pins
    GPIOC->MODER |= 0x5555; // Output mode for GPIOC pins

    while (1) {
        // Display the current values of 'a' and 'b' on the 7-segment display 
				int dipS = (GPIOA -> IDR >> 4) & 0x07;

				switch (dipS){
					default:
					case 0x0:
						dispNum(b,a);
						break;
					case 0x1:
						//sumar conteos
						spDig(num);
						break;
					case 0x2:
						//restar conteos
						GPIOB->ODR = dp[2];
						GPIOC->ODR = dp[2];
						break;
					case 0x3:
						//promedios
						spDig(num/res);
						break;
					case 0x4:
						// varianza
						Varz(numarr,res);						
						break;
					case 0x5:
						// moda conteos
						calcModa(numarr,res);
						break;
					case 0x6:
						//reinicio conteos
						for(int x=0; x<100;x++){numarr[x]=0;}
						num=0;
						res=0;
						a=0;
						b=0;
						break;
					case 0x7:
						//numero de conteos
						spDig(res);
						break;
				
				}
			
    }
}