#include <Servo.h>

#define TRIG 9
#define ECHO 10
#define PIN_ENTRADA_7 7
#define PIN_SALIDA_8 8
#define PIN_ENTRADA_2 2

Servo servo;
Servo servo2;

bool objetoDetectado = false;  
bool esperandoObjetoSalir = false;  
unsigned long tiempoInicio = 0;
const int TIEMPO_ESPERA = 1000; // 20 segundos

// Para controlar el segundo servomotor
bool segundoServoActivo = false;
unsigned long tiempoSegundoServo = 0;

void setup() {
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(PIN_ENTRADA_7, INPUT);
  pinMode(PIN_SALIDA_8, OUTPUT);
  pinMode(PIN_ENTRADA_2, INPUT);

  servo.attach(6);      // Primer servomotor (barrera)
  servo2.attach(5);     // Segundo servomotor

  servo.write(0);       // Barrera abajo
  servo2.write(0);      // Segundo servo en posición inicial

  Serial.begin(9600);
}

void loop() {
  // ----------------------------
  // FUNCIONALIDAD 1: Sensor ultrasónico + barrera
  // ----------------------------
  // Medir la distancia
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  int distance = duration * 0.034 / 2; // Convertir tiempo a distancia en cm
  
  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Si detecta un objeto a 5 cm o menos y la barrera no está arriba ni esperando que el objeto se retire
  if (distance <= 5 && !objetoDetectado && !esperandoObjetoSalir) {  
    Serial.println("🚗 Objeto detectado a 5 cm. Levantando la barrera...");
    servo.write(90);  // Sube la barrera
    objetoDetectado = true;
    tiempoInicio = millis();  // Guarda el tiempo en que la barrera subió
  }

  // Si la barrera está arriba y han pasado 20 segundos, bajarla
  if (objetoDetectado && millis() - tiempoInicio >= TIEMPO_ESPERA) {
    Serial.println("⬇ Bajando la barrera...");
    servo.write(0);  // Baja la barrera
    esperandoObjetoSalir = true;  // Ahora espera que el objeto se retire
    objetoDetectado = false;  // Resetea la detección de objeto
  }

  // Espera a que el objeto se quite antes de volver a detectar otro
  if (esperandoObjetoSalir && distance > 5) {  
    Serial.println("✅ Objeto retirado. Sistema listo para detectar otro vehículo.");
    esperandoObjetoSalir = false;  // Reinicia el sistema
  }

  delay(100); // Pequeño retraso para evitar lecturas erráticas

  // ----------------------------
  // FUNCIONALIDAD 2: Si pin 7 está en HIGH → activar pin 8 por 0.5 segundos
  // ----------------------------
  if (digitalRead(PIN_ENTRADA_7) == HIGH) {
    Serial.println("⚡ Entrada en pin 7 detectada. Activando pin 8 por 0.5 segundos.");
    digitalWrite(PIN_SALIDA_8, HIGH);
    delay(500);
    digitalWrite(PIN_SALIDA_8, LOW);
  }

  // ----------------------------
  // FUNCIONALIDAD 3: Segundo servo activado por pin 2
  // ----------------------------
  if (digitalRead(PIN_ENTRADA_2) == HIGH && !segundoServoActivo) {
    Serial.println("🟢 Pin 2 activado. Girando segundo servomotor.");
    servo2.write(90); // Gira a 90 grados
    tiempoSegundoServo = millis();
    segundoServoActivo = true;
  }

  if (segundoServoActivo && millis() - tiempoSegundoServo >= 5000) {
    Serial.println("🔴 Cerrando segundo servomotor después de 5 segundos.");
    servo2.write(0); // Vuelve a 0 grados
    segundoServoActivo = false;
  }

  delay(100);
}
