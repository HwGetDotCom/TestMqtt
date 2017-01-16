////////////////////////////////////////////////////////////////////////////////
//  #######################################################################   //
//  #  Programa para probar la conexión del ESP8266 con un servidor MQTT  #   //
//  #######################################################################   //
//                                                                            //
//     Descripción: Este programa se conecta a un red Wifi y despues          //
//     establece la comunicación con un servidor MQTT en el que puede         //
//     publicar o suscribirse a un canal para enviar y recibir mensajes       //
//     respectivamente                                                        //
//                                                                            //
//  Creado por: Rodolfo Tiburcio Calles                                       //
//  Compañia: www.hw-get.com                                                  //
//  eMail: contacto@hw-get.com                                                //
/******************************************************************************/

//////////// Indicamos las librerías necesarias para el programa ///////////////
#include "Arduino.h"        //Agregamos la librería de arduino
#include <ESP8266WiFi.h>    //Agregamos la librería del ESP8266
#include <PubSubClient.h>   //Libreria para manejar los mensajes MQTT
/******************************************************************************/

//////////// Indicamos las credenciales para accedera la red wifi //////////////
const char* ssid = "hw-get";         //Nombre de la red
const char* password = "hw-get.Com"; //Contraseña
/******************************************************************************/

///////////// Declaraciones necesarias para los mensajes MQTT //////////////////
const char* mqtt_server = "broker.mqtt-dashboard.com";  //Servidor MQTT
WiFiClient espClient;             //Creamos un cliente Wifi
PubSubClient client(espClient);   //Indicamos que usaremos ese cliente para MQTT
long lastMsg = 0;                 //Variable para medir tiempo entre mensajes
char msg[50];                     //Variable donde se se escribirá el mensaje
/******************************************************************************/

///////// Declaramos los prototipos de las funciones que utilizaremos //////////
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
/******************************************************************************/

//////// Declaramos y definimos la función de configuración "setup" ////////////
void setup(){                  //Declaramos la función de configuración "setup"
  pinMode(BUILTIN_LED, OUTPUT);     //Configuramos al led integrado como salida
  Serial.begin(115200);        //La velocidad de la comunicación serial a 115200
  setup_wifi();               //Llamamos a la funcion de configuracion del WiFi
  client.setServer(mqtt_server,1883);  //Indicamos el servidor y el puerto MQTT
  client.setCallback(callback);  //Indicamos la función para mensajes recibidos
}
/******************************************************************************/

///////////// Definimos la función de configuración "setup_wifi" ///////////////
void setup_wifi(){        //Definimos lo que hará la función "setup_wifi"
  delay(10);              //Creamos un retardo de 10 ms
  Serial.println();       //Mandamos por comunicación serial un salto de línea
  Serial.print("Conectando a ");  //Mandamos el texto "Conectando a "
  Serial.println(ssid);   //Mandamos en nombre de la red
  WiFi.begin(ssid, password);   //llamamos a la función WiFi.begin indicando
                                //el nombre y la contraseña de la red
  while(WiFi.status()!=WL_CONNECTED){ //Ciclo hasta establecer la conexión
    delay(500);                       //Un retardo de 0.5 seg.
    Serial.print(".");                //Se envía un carácter cada ciclo
  }                                   //Una vez conectado se sale del ciclo
  Serial.println("");                 //y se indica en la consola que la
  Serial.printf("WiFi conectado");    //comunicación ya fue establecida
  Serial.println("Dirección IP: ");   //Indica la dirección IP actual
  Serial.println(WiFi.localIP());     //asignada al módulo
}
/******************************************************************************/

////////////////////// Definimos la función "callback" /////////////////////////
void callback(char* topic, byte* payload, unsigned int length){
  Serial.print("Mensaje recibido ["); //Indicamos que hemos recibido un mensaje
  Serial.print(topic);                //y en qué canal fue recibido
  Serial.print("]");
  for(int i =0; i < length;i++){      //Escribimos el mensaje carácter por
    Serial.print((char)payload[i]);   //carácter
  }
}
/******************************************************************************/

////////////////////// Definimos la función "reconnect" ////////////////////////
void reconnect(){
  while(!client.connected()){    //Ciclo si no hay conexión con el servidor MQTT
    Serial.print("Intentando establecer conexión MQTT"); //Indicamos el estado actual
    if(client.connect("Nombre_Asignado_AlModulo")){ //Si el cliente ya se ha conectado (Nota:El módulo ESP8266 es el cliente)
      Serial.println("conectado!");            //Indicamos que ya está conectado
      client.publish("NombreDelCanal", "Mensaje para publicar"); //Publicamos un mensaje en un canal
      client.subscribe("NombreDelCanal");           //Nos suscribimos a un canal
    }else {                                        //Si aún no hay conexión MQTT
      Serial.print("Error, rc=");    //Indicamos que hay un error en la conexión
      Serial.print(client.state()); //Mandamos el estado actual del cliente MQTT
      Serial.println(" Reintentando conexión en 5 seg.");     //Indicamos que se
      delay(5000);                        //realizará un nuevo intento en 5 seg.
    }
  }
}
/******************************************************************************/

/////////////////////// Definimos la función "loop" ////////////////////////////
void loop(){
  if(!client.connected()){    //Si no está conectado el cliente MQTT
    reconnect();              //Se llama a la función "reconnect()"
  }
  client.loop();              //Función para que el ESP8266 sea cliente MQTT
  long now = millis();        //Guardamos el momento actual en la variable now
  if(now-lastMsg>2000){       //Si el último mensaje fue hace más de 2 seg.
    lastMsg=now;              //Actualizamos el tiempo para el último mensaje
    snprintf(msg,75,"%ld",1); //Componemos el mensaje que queremos publicar
    client.publish("NombreDelCanal",msg); //Publicamos el mensaje en el canal
  }                                       //seleccionado
}
/******************************************************************************/
