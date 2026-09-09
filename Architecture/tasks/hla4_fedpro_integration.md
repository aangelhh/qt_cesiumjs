# Integrar HLA 4 mediante FedProClient

Estado: pendiente de implementacion.
Fecha: 2026-09-09.
Prioridad: siguiente ampliacion de interoperabilidad HLA.

## Objetivo

Permitir que qttest participe en una federacion mediante la API C++ HLA 4
(IEEE 1516-2025) de Pitch FedProClient, como backend opcional seleccionable.
Conservar los backends IEEE 1516e existentes y el funcionamiento DIS.

Referencia: https://github.com/Pitch-Technologies/FedProClient/tree/main

El README del proveedor identifica variantes C++ para HLA Evolved y HLA 4,
y licencia Apache-2.0. Elegir explicitamente la variante HLA 4; conectar la
API Evolved por Federate Protocol no constituye por si solo esta integracion.
El cliente necesita un servidor Federate Protocol compatible. Confirmar la
version y configuracion del RTI disponible antes de dar por valida la conexion.

## Trabajo

- [ ] Fijar una revision de FedProClient y revisar licencia, dependencias,
  instrucciones CMake, compilador y compatibilidad macOS/arquitectura actual.
- [ ] Compilar y ejecutar el ejemplo C++ HLA 4 contra un servidor compatible;
  registrar versiones del cliente y servidor, endpoint y resultado reproducible.
- [ ] Incorporar una opcion CMake independiente y un plugin `fedpro-hla4`.
  Aislar bibliotecas y simbolos RTI para evitar conflictos con IEEE 1516e.
- [ ] Adaptar `IHlaBackend`, `HlaBackendPluginApi` y el registro de backends
  siguiendo las convenciones actuales. Ampliar contratos solo donde sea necesario.
- [ ] Implementar connect, create/join, resign/disconnect, FOM modular,
  publicacion/suscripcion, registro/actualizacion/eliminacion de objetos e
  interacciones, reutilizando los publicadores RPR existentes cuando sean compatibles.
- [ ] Adaptar callbacks, handles y errores. Mantener Qt responsivo y controlar
  la vida de callbacks al cerrar o reconectar. No anunciar capacidades no soportadas.
- [ ] Cubrir puntos de sincronizacion, regulacion y restriccion temporal,
  avance de tiempo, timestamps y propiedad de atributos segun el contrato actual.
- [ ] Anadir seleccion del backend y configuracion persistente del endpoint
  FedPro en inicio y panel HLA, con diagnosticos de conexion claros.
- [ ] Validar entidades en movimiento, radar, armamento y controles de simulacion
  entre dos federados, incluidos borrados y filtrado de ecos.
- [ ] Probar perdida de conexion, reconexion, cierre y errores de configuracion;
  especificar que servicios HLA 4 nuevos quedan fuera del primer incremento.
- [ ] Documentar compilacion, configuracion del servidor y ejecucion de pruebas.

## Criterios De Aceptacion

1. Se selecciona HLA 4 desde la aplicacion y se conecta a un servidor real.
2. Dos federados intercambian objetos e interacciones con datos verificables,
   incluyendo posicion cambiante y retirada de objetos.
3. Las capacidades temporales y de propiedad declaradas tienen pruebas;
   las no implementadas producen un resultado explicito, nunca exito ficticio.
4. La interfaz sigue respondiendo durante conexion, callbacks y desconexion.
5. Compila sin FedProClient cuando la opcion esta desactivada y pasan las
   pruebas de regresion HLA Evolved y DIS.
6. La prueba con RTI externo se distingue de las pruebas mock; no se marca
   completa la integracion usando solo compilacion o dobles de prueba.

## Puntos De Entrada

- `src/infrastructure/interoperability/hla/IHlaBackend.h`
- `src/infrastructure/interoperability/hla/HlaBackendPluginApi.h`
- `src/infrastructure/interoperability/hla/HlaBackendFactory.cpp`
- `integrations/hla/pitch1516e/Pitch1516eBackendPlugin.cpp`
- `src/application/HlaStartupSession.cpp`
- `src/application/StartupConfiguration.h`
- `src/presentation/HlaConnectionPanel.cpp`
- `src/presentation/StartupConfigurationDialog.cpp`
- `CMakeLists.txt` y `tests/infrastructure/`

## Limites

Esta tarea no sustituye el RTI servidor ni presupone que el pRTI instalado
exponga FedPro. Su compatibilidad se verificara en la primera fase.
No incluye nuevas funciones de simulacion ni migracion obligatoria de los
usuarios IEEE 1516e. La revision de planificacion no ha compilado FedProClient.
