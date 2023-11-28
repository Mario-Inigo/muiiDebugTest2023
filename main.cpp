#include "mbed.h"
#include "datos.h"
#include "math.h"
#include <cstdint>


#define longitudTrama 200
#define fs (50*20)

Timer timer;
float resultado;
int tiempo;

struct estructuraMedidas {
  float vrms;
  float irms;
  float potenciaActiva;
  float potenciaReactiva;
  float potenciaAparente;
  float factorDePotencia;
  float energiaConsumida;
};

float calcularRMS(uint16_t *datos, int longitud);

void calcularDatos(uint16_t *datosV, uint16_t *datosI, int longitud, estructuraMedidas *medidas);

int main() {

  timer.reset(); //Reinicia el temporizado
  timer.start();
  resultado = calcularRMS(datos, longitudTrama);
  timer.stop();
  printf("\n****El valor Vrms es %f calculado en %lld us ****\n\n", resultado,
         timer.elapsed_time().count());

  estructuraMedidas medidas;
  medidas.energiaConsumida = 0;

  timer.reset();
  timer.start();
  calcularDatos(datosV, datosI, longitudTrama, &medidas);
  timer.stop();
  printf("**** Datos calculados en %lld us ****\n",
         timer.elapsed_time().count());
  printf("**** El valor Vrms es %f ****\n", medidas.vrms);
  printf("**** El valor Irms es %f ****\n", medidas.irms);
  printf("**** La potencia activa es %f ****\n", medidas.potenciaActiva);
  printf("**** La potencia reactiva es %f ****\n", medidas.potenciaReactiva);
  printf("**** La potencia aparente es %f ****\n", medidas.potenciaAparente);
  printf("**** La energia consumida es %f ****\n", medidas.energiaConsumida);
  printf("**** El factor de potencia es es %f ****\n", medidas.factorDePotencia);

  while (true) {
  }
}

// Esta función calcula el valor RMS
float calcularRMS(uint16_t *datos, int longitud) {
    const float a = 800.0 / 65536.0;
    const float b = -400.0;

    intmax_t rms = 0;

    for (int i = 0; i < longitud; ++i) {
        rms += (int)datos[i] * (int)datos[i];
    }
    
    return sqrt(rms*a*a/longitud +b*(-b));
}


void calcularDatos(uint16_t *datosV, uint16_t *datosI, int longitud, estructuraMedidas *medidas) {
    const float aV = 800.0 / 65536.0;
    const float bV = -400.0;
    const float aI = 5.0 / 65536.0;
    const float bI = -2.5;
 
 
    intmax_t rms_i = 0;
    intmax_t rms_v = 0;
    intmax_t p_calculo = 0;

    for (int i = 0; i < longitud; ++i) {
        rms_v += (int)datosV[i] * (int)datosV[i];
        rms_i += (int)datosI[i] * (int)datosI[i];
        p_calculo += (int)datosV[i] * (int)datosI[i];
    }
    

    float Vrms = sqrt(rms_v * aV *aV / longitud + bV*(-bV));
    float Irms = sqrt(rms_i * aI * aI/ longitud + bI*(-bI));
    float P = p_calculo * aV * aI / longitud - (-bV)*(-bI);

    float S = Vrms * Irms;
    float Q = sqrt(S * S - P * P);
    float FA = P / S;
    float E = P /fs*longitudTrama/60/60/1000;

    // Asignar los resultados a la estructura de medidas
    medidas->vrms = Vrms;
    medidas->irms = Irms;
    medidas->potenciaActiva = P;
    medidas->potenciaAparente = S;
    medidas->potenciaReactiva = Q;
    medidas->factorDePotencia = FA;
    medidas->energiaConsumida += E;
}