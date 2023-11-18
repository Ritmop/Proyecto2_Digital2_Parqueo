// Proyecto #3 
// Electrónica Digital II
//Judah Pérez y Carlos Valdez

// Programación para recibir información sobre 8 parqueos por medio de UART
// Según los datos recibidos se actualiza un servidor web mostrando los cambios

// Librerías
//************************************************************************************************
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HardwareSerial.h>

// put function declarations here:
void handle_actualizar();
void handle_OnConnect();
void handle_NotFound();
void procesarDatos(char data);
void display(int num);

//************************************************************************************************
// Variables globales

//Variables booleanas para cada parqueo
bool bit0, bit1, bit2, bit3, bit4, bit5, bit6, bit7;

// variable que recibe los datos de la Tiva 1
char data;
// variable que recibe los datos de la Tiva 2
//char data1;

// SSID & Password
const char* ssid = "danielvaldez";  // Enter your SSID here
const char* password = "daniel21976";  //Enter your Password here

const int pin7s[] = {15,2,4,5,18,19,21};  //pines para display de 7 segmentos (a,b,c,d,e,f,g)
const char prueba[] = {0,1,3,7,15,31,63,127,255};

int a = 15;
int b = 2;
int c = 4;
int d = 5;
int e = 18;
int f = 19;
int g = 21;

WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)

HardwareSerial MySerial1(1);  //Se define un serial para UART1
const int Serial1RX = 16;
const int Serial1TX = 17;

/*#define RX_PIN 16  // Puedes cambiar estos pines según tu conexión
#define TX_PIN 17
*/

const int totalParqueos = 8;

int cantidadBitsTrue;
int resultado;


//el siguiente código se usaría para agregar otro UART
/*HardwareSerial MySerial2(1);  //Se define un serial para UART1
const int Serial2RX = 32;
const int Serial2TX = 33;*/


//************************************************************************************************
// Procesador de HTML
//************************************************************************************************
String SendHTML(){
String ptr = "<!DOCTYPE html>\n";                                                                                                                                                          
ptr += "<html lang=\"es\">\n";                                                                                                                                                                             
ptr += "<head>\n";                                                                                                                                                                         
ptr += "  <meta charset=\"UTF-8\">\n";
ptr += "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">\n";
ptr +="  <title>Parqueo JD</title>\n";
ptr +="  <!-- Vincula las hojas de estilo de Bootstrap -->\n";
ptr +="  <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">\n";
ptr +="  <style>\n";
ptr +="    body {\n";
ptr +="      background: linear-gradient(to left, #333, #000); /* Fondo oscuro en gradiente */\n";
ptr +="      color: ;#fff; /* Color de texto blanco */\n";
ptr +="      text-align: center;\n";
ptr +="      font-family: 'Arial', sans-serif; /* Tipo de letra */\n";
ptr +="    }\n";
ptr +="\n";
ptr +="    h1 {\n";
ptr +="      margin-top: 50px; /* Espaciado superior del título */\n";
ptr +="    }\n";
ptr +="\n";
ptr +="    .progress {\n";
ptr +="      width: 70%; /* Ancho de la barra de progreso */\n";
ptr +="      margin: 30px auto; /* Centrar la barra de progreso */\n";
ptr +="    }\n";
ptr +="\n";
ptr +="    button {\n";
ptr +="      margin-top: 20px; /* Espaciado superior del botón */\n";
ptr +="      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* Sombra del botón */\n";
ptr +="    }\n";
ptr +="\n";
ptr +="    table {\n";
ptr +="      width: 80%; /* Ancho de la tabla */\n";
ptr +="      margin: 30px auto; /* Centrar la tabla */\n";
ptr +="      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); /* Sombra de la tabla */\n";
ptr +="    }\n";
ptr +="\ttable th, table td {\n";
ptr +="    color: #fff; /* Color de texto blanco para las celdas de la tabla */}\n";
ptr +="\n";
ptr +="\tth.numero-parqueo, td.numero-parqueo {\n";
ptr +="\twidth: 50%; /* Ancho igual para ambas columnas */\n";
ptr +="    text-align: center; /* Alinea el texto a la izquierda para la columna \"Número de Parqueo\" */\n";
ptr +="    color: #fff; /* Color de texto blanco para las celdas de esta columna */\n";
ptr +="  }\n";
ptr +="\n";
ptr +="  \tth.estado, td.estado {\n";
ptr +="\twidth: 50%; /* Ancho igual para ambas columnas */\n";
ptr +="    text-align: center; /* Alinea el texto a la derecha para la columna \"Estado\" */\n";
ptr +="    color: #fff; /* Color de texto blanco para las celdas de esta columna */\n";
ptr +="  }\n";
ptr +="\n";
ptr +="  .contadorD {\n";
ptr +="    position: absolute;\n";
ptr +="    top: 50px;\n";
ptr +="    left: 20px;\n";
ptr +="    color: #fff;\n";
ptr +="  }\n";
ptr +="  .contadorO {\n";
ptr +="    position: absolute;\n";
ptr +="    top: 80px;\n";
ptr +="    left: 20px;\n";
ptr +="    color: #fff;\n";
ptr +="  }\n";
ptr +="\n";
ptr +="  </style>\n";
ptr +="</head>\n";
ptr +="<body>\n";
ptr +="\n";
ptr +="  <div class=\"container\">\n";
ptr +="    <h1><font color=white>Parqueo JD &#x1F17F🚗 </h1>\n";
ptr +="\n";
ptr +="    <!-- Barra de progreso -->\n";
ptr +="    <div id=\"barraProgreso\" class=\"progress\">\n";
ptr +="    <div class=\"progress-bar progress-bar-striped bg-danger\" role=\"progressbar\" aria-valuenow=\"0\" aria-valuemin=\"0\" aria-valuemax=\"100\">  \n";
ptr +="    </div>\n";
ptr +="      </div>\n";
ptr +="\n";
ptr +="    <!-- Botón de actualizar -->\n";
ptr +="    <button type=\"button\" class=\"btn btn-primary btn-lg\" href=\"/actualizar\">Actualizar</button>\n";
ptr +="\n";
ptr +="    <div class=\"contadorO\" id=\"contadorOcupados\">Parqueos Ocupados: </div>\n";
ptr +="    <div class=\"contadorD\" id=\"contadorLibres\">Parqueos Libres: </div>\n";
ptr +="\n";
ptr +="    <!-- Tabla de parqueos -->\n";
ptr +="    <table class=\"table table-striped\">\n";
ptr +="      <thead>\n";
ptr +="        <tr>\n";
ptr +="          <th class=\"numero-parqueo\" scope=\"col\">Número de Parqueo</th>\n";
ptr +="          <th class=\"estado\" scope=\"col\">Estado</th>\n";
ptr +="        </tr>\n";
ptr +="      </thead>\n";
ptr +="      <tbody>\n";
ptr +="        <tr>\n";
ptr +="          <td class=\"numero-parqueo\">1</td>\n";
ptr +="          <td class=\"estado\" id=\"par1\"></td>\n";
ptr +="        </tr>\n";
ptr +="        <tr>\n";
ptr +="          <td class=\"numero-parqueo\">2</td>\n";
ptr +="          <td class=\"estado\" id=\"par2\"></td>\n";
ptr +="        </tr>\n";
ptr +="        <tr>\n";
ptr +="          <td class=\"numero-parqueo\">3</td>\n";
ptr +="          <td class=\"estado\" id=\"par3\"></td>\n";
ptr +="        </tr>\n";
ptr +="        <tr>\n";
ptr +="          <td class=\"numero-parqueo\">4</td>\n";
ptr +="          <td class=\"estado\" id=\"par4\"></td>\n";
ptr +="        </tr>\n";
ptr +="        <tr>\n";
ptr +="          <td class=\"numero-parqueo\">5</td>\n";
ptr +="          <td class=\"estado\" id=\"par5\"></td>\n";
ptr +="        </tr>\n";
ptr +="        <tr>\n";
ptr +="          <td class=\"numero-parqueo\">6</td>\n";
ptr +="          <td class=\"estado\" id=\"par6\"></td>\n";
ptr +="        </tr>\n";
ptr +="        <tr>\n";
ptr +="          <td class=\"numero-parqueo\">7</td>\n";
ptr +="          <td class=\"estado\" id=\"par7\"></td>\n";
ptr +="        </tr>\n";
ptr +="        <tr>\n";
ptr +="          <td class=\"numero-parqueo\">8</td>\n";
ptr +="          <td class=\"estado\" id=\"par8 \"></td>\n";
ptr +="        </tr>\n";
ptr +="      </tbody>\n";
ptr +="    </table>\n";
ptr +="  </div>\n";
ptr +="\n";
ptr +="  <!-- Vincula los scripts de Bootstrap y jQuery (necesario para algunas funciones de Bootstrap) -->\n";
ptr +="  <script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\"></script>\n";
ptr +="  <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\"></script>\n";
ptr +="  <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\"></script>\n";
ptr +="\n";
ptr +="</body>\n";
ptr +="</html>\n";
ptr +="\n";
ptr +="<script>\n";

if (bit0)
  {
    ptr +="  var P1 = 1;\n";
  }
  else
  {
    ptr +="  var P1 = 0;\n";
  }
if (bit1)
  {
    ptr +="  var P2 = 1;\n";
  }
  else
  {
    ptr +="  var P2 = 0;\n";
  }  

if (bit2)
  {
    ptr +="  var P3 = 1;\n";
  }
  else
  {
    ptr +="  var P3 = 0;\n";
  }  

if (bit3)
  {
    ptr +="  var P4 = 1;\n";
  }
  else
  {
    ptr +="  var P4 = 0;\n";
  }

if (bit4)
  {
    ptr +="  var P5 = 1;\n";
  }
  else
  {
    ptr +="  var P5 = 0;\n";
  }  

if (bit5)
  {
    ptr +="  var P6 = 1;\n";
  }
  else
  {
    ptr +="  var P6 = 0;\n";
  }  

if (bit6)
  {
    ptr +="  var P7 = 1;\n";
  }
  else
  {
    ptr +="  var P7 = 0;\n";
  }  

if (bit7)
  {
    ptr +="  var P8 = 1;\n";
  }
  else
  {
    ptr +="  var P8 = 0;\n";
  }  

ptr +="  // Ejemplo de valores iniciales\n";
ptr +="  var parqueosOcupados = P1 + P2+ P3 + P4 + P5 + P6 + P7 + P8 ;\n";
ptr +="  var totalParqueos = 8;\n";
ptr +="  var parqueosLibres = (totalParqueos - parqueosOcupados);\n";
ptr +="  // Calcula el porcentaje de parqueos ocupados\n";
ptr +="  var porcentajeOcupados = (parqueosOcupados / totalParqueos) * 100;\n";
ptr +="  // Actualiza los contadores\n";
ptr +="  var estadoCelda1 = P1 === 1 ? \"Ocupado\" : \"Libre\" \n";
ptr +="  var estadoCelda2 = P2 === 1 ? \"Ocupado\" : \"Libre\" \n";
ptr +="  var estadoCelda3 = P3 === 1 ? \"Ocupado\" : \"Libre\" \n";
ptr +="  var estadoCelda4 = P4 === 1 ? \"Ocupado\" : \"Libre\" \n";
ptr +="  var estadoCelda5 = P5 === 1 ? \"Ocupado\" : \"Libre\" \n";
ptr +="  var estadoCelda6 = P6 === 1 ? \"Ocupado\" : \"Libre\" \n";
ptr +="  var estadoCelda7 = P7 === 1 ? \"Ocupado\" : \"Libre\" \n";
ptr +="  var estadoCelda8 = P8 === 1 ? \"Ocupado\" : \"Libre\" \n";
ptr +="  document.getElementById(\"contadorOcupados\").innerText = \"Parqueos Ocupados: \" + parqueosOcupados;\n";
ptr +="  document.getElementById(\"contadorLibres\").innerText = \"Parqueos Libres: \" + parqueosLibres;\n";
ptr +="\n";
ptr +="  // Actualiza la barra de progreso\n";
ptr +="  var barraProgreso = document.getElementById(\"barraProgreso\").firstElementChild;\n";
ptr +="  barraProgreso.style.width = porcentajeOcupados + \"%\";\n";
ptr +="  barraProgreso.innerText = porcentajeOcupados + \"% Ocupados\";\n";
ptr +="  document.getElementById(\"par1\").innerText = estadoCelda1;\n";
ptr +="  document.getElementById(\"par2\").innerText = estadoCelda2;\n";
ptr +="  document.getElementById(\"par3\").innerText = estadoCelda3;\n";
ptr +="  document.getElementById(\"par4\").innerText = estadoCelda4;\n";
ptr +="  document.getElementById(\"par5\").innerText = estadoCelda5;\n";
ptr +="  document.getElementById(\"par6\").innerText = estadoCelda6;\n";
ptr +="  document.getElementById(\"par7\").innerText = estadoCelda7;\n";
ptr +="  document.getElementById(\"par8\").innerText = estadoCelda8;\n";
ptr +="</script>\n";
ptr +="\n";
return ptr;
}


//************************************************************************************************
// Configuración
//************************************************************************************************
void setup() {
  Serial.begin(115200);

  //Serial1.begin(115200, SERIAL_8N1, 16, 17, false, 20000UL, 112); //Habilitar el UART1 y cambiar los pins
  Serial.println("Try Connecting to ");
  Serial.println(ssid);
  MySerial1.begin(115200, SERIAL_8N1, Serial1RX, Serial1TX);


    pinMode(15, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(18, OUTPUT);
    pinMode(19, OUTPUT);
    pinMode(21, OUTPUT);

    digitalWrite(15, LOW);
    digitalWrite(2, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(18, LOW);
    digitalWrite(19, LOW);
    digitalWrite(21, LOW);

  /*for (int i = 0; i < 8; i++) {
    digitalWrite(pin7s[i], LOW);
  }*/




  // Connect to your wi-fi modem
  WiFi.begin(ssid, password);

  // Check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected successfully");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());  //Show ESP32 IP on serial

  server.on("/", handle_OnConnect); // Directamente desde e.g. 192.168.0.8
  server.on("/actualizar", handle_actualizar);
  
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
  delay(100);
}
//************************************************************************************************
// loop principal
//************************************************************************************************
void loop() {
  server.handleClient();

/*if (MySerial1.available()) {
    data = MySerial1.read();  // Leer un byte de datos de UART1
    //data = 3;

    // Procesar el byte de datos
    
  }*/

  for (int i=0; i < 8; i++){
    data = prueba[i];
    server.handleClient();
    Serial.println("data = ");
    Serial.println(data);
    procesarDatos(data);
    display(resultado);
    handle_actualizar();
    delay(5000);
  }


  //data++;
 
  
}
//************************************************************************************************
// Handler de actualizacion
//************************************************************************************************
void handle_actualizar() {
  Serial.println("Actualizar pagina");
  server.send(200, "text/html", SendHTML());
}


// Handler de Inicio página
//************************************************************************************************
void handle_OnConnect() {
  Serial.println("server up");
  server.send(200, "text/html", SendHTML());
}
//************************************************************************************************
// Handler de not found
//************************************************************************************************
void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

void handleP1(){
  if (bit0 == false) {
    server.send(200, "text/plain", "0"); //Si el parqueo esta vacío enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  } else {
    server.send(200, "text/plain", "1"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  }
}

void handleP2(){
  if (bit1 == false) {
    server.send(200, "text/plain", "0"); //Si el parqueo esta vacío enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  } else {
    server.send(200, "text/plain", "1"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  }
}

void handleP3(){
  if (bit2 == false) {
    server.send(200, "text/plain", "0"); //Si el parqueo esta vacío enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  } else {
    server.send(200, "text/plain", "1"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  }
}

void handleP4(){
  if (bit3 == false) {
    server.send(200, "text/plain", "0"); //Si el parqueo esta vacío enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  } else {
    server.send(200, "text/plain", "1"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  }
}

void handleP5(){
  if (bit4 == false) {
    server.send(200, "text/plain", "0"); //Si el parqueo esta vacío enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  } else {
    server.send(200, "text/plain", "1"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  }
}

void handleP6(){
  if (bit5 == false) {
    server.send(200, "text/plain", "0"); //Si el parqueo esta vacío enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  } else {
    server.send(200, "text/plain", "1"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  }
}
void handleP7(){
  if (bit6 == false) {
    server.send(200, "text/plain", "0"); //Si el parqueo esta vacío enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  } else {
    server.send(200, "text/plain", "1"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  }
}

void handleP8(){
  if (bit7 == false) {
    server.send(200, "text/plain", "0"); //Si el parqueo esta vacío enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  } else {
    server.send(200, "text/plain", "1"); //Si el parqueo esta ocupado enviar un texto a javascript con la clase de css en HTML para asignarle al id correspondiente
  }
}

void procesarDatos(char bitsp) {

    // Se aplica una máscara para extraer cada bit recibido de la tiva
    bit0 = (bitsp & 0b00000001) != 0;
    bit1 = (bitsp & 0b00000010) != 0;
    bit2 = (bitsp & 0b00000100) != 0;
    bit3 = (bitsp & 0b00001000) != 0;
    bit4 = (bitsp & 0b00010000) != 0;
    bit5 = (bitsp & 0b00100000) != 0;
    bit6 = (bitsp & 0b01000000) != 0;
    bit7 = (bitsp & 0b10000000) != 0;

    // obtener el número de parqueos libres
    cantidadBitsTrue = bit0 + bit1 + bit2 + bit3 + bit4 + bit5 + bit6 + bit7;
    resultado = totalParqueos - cantidadBitsTrue;

    Serial.println("parqueos disponibles");
    Serial.println("p1");
    Serial.println(bit0);
    Serial.println("p2");
    Serial.println(bit1);
    Serial.println("p3");
    Serial.println(bit2);
    Serial.println("p4");
    Serial.println(bit3);
    Serial.println("p5");
    Serial.println(bit4);
    Serial.println("p6");
    Serial.println(bit5);
    Serial.println("p7");
    Serial.println(bit6);
    Serial.println("p8");
    Serial.println(bit7);
    Serial.println("Cantidad de Parqueos disponibles");
    Serial.println(resultado);
    delay(500);
}

void display(int num) {
  // Tabla de mapeo para mostrar dígitos en el display de 7 segmentos
  const byte segmentMapping[] = {
    B11111100,  // 0
    B01100000,  // 1
    B11011010,  // 2
    B11110010,  // 3
    B01100110,  // 4
    B10110110,  // 5
    B10111110,  // 6
    B11100000,  // 7
    B11111110,  // 8
    B11110110   // 9
  };

  // Validar el número para asegurarse de que esté en el rango adecuado
  if (num >= 0 && num <= 9) {
    // Apagar todos los segmentos
    for (int i = 0; i < 7; i++) {
      digitalWrite(pin7s[i], LOW);
    }

    // Encender los segmentos correspondientes al número
    /*byte segmentData = segmentMapping[num];
    for (int i = 0; i < 7; i++) {
      digitalWrite(pin7s[i], (segmentData >> i) & 1);
    }*/

switch (num){
    case 0:
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, LOW);
break;

case 1:
  digitalWrite(a, LOW);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);
break;

case 2: 
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, LOW);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, LOW);
  digitalWrite(g, HIGH);
break;

case 3:
  digitalWrite(a, HIGH); 
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, HIGH);
break;

case 4:
  digitalWrite(a, LOW);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
break;

case 5:
  digitalWrite(a, HIGH);
  digitalWrite(b, LOW);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
break;

case 6:
  digitalWrite(a, HIGH);
  digitalWrite(b, LOW);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
break;

case 7:
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);
break;

case 8:
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH); 
break;

case 9:
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, LOW); 
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
break;
    default:
     num = -1; //Delimitador -1
    break;
  }

  }
}