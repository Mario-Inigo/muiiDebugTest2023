#include "mbed.h"
#include "datos.h"
#include "math.h"


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

    float rms = 0.0;

    for (int i = 0; i < longitud; ++i) {
        float datoV = (static_cast<float>(datos[i]) * a) + b;
        rms += datoV * datoV;
    }

    return std::sqrt(rms / static_cast<float>(longitud));
}

void calcularDatos(uint16_t *datosV, uint16_t *datosI, int longitud, estructuraMedidas *medidas) {
    const float aV = 800.0 / 65536.0;
    const float bV = -400.0;
    const float aI = 5.0 / 65536.0;
    const float bI = -2.5;

    float Vrms = 0.0;
    float Irms = 0.0;
    float P = 0.0;
    float S = 0.0;
    float Q = 0.0;
    float FA = 0.0;
    float E = 0.0;

    for (int i = 0; i < longitud; ++i) {
        float datoV = ((static_cast<float>(datosV[i]) * aV) + bV);
        float datoI = ((static_cast<float>(datosI[i]) * aI) + bI);

        Vrms += datoV * datoV;
        Irms += datoI * datoI;
        P += datoV * datoI;
    }

    Vrms = std::sqrt(Vrms / static_cast<float>(longitud));
    Irms = std::sqrt(Irms / static_cast<float>(longitud));
    P /= static_cast<float>(longitud);

    S = Vrms * Irms;
    Q = std::sqrt(S * S - P * P);

    if (S > 0) {
        FA = P / S;
    } else {
        FA = 0.0; // Para el caso de S == 0 para evitar divisiones por cero
    }

    E = P / fs * longitudTrama / 60 / 60 / 1000;

    // Asignar los resultados a la estructura de medidas
    medidas->vrms = Vrms;
    medidas->irms = Irms;
    medidas->potenciaActiva = P;
    medidas->potenciaAparente = S;
    medidas->potenciaReactiva = Q;
    medidas->factorDePotencia = FA;
    medidas->energiaConsumida += E;
}
