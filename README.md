# qttest

Visor base en C++ con Qt 6 que carga `CesiumJS` dentro de `Qt WebEngine`.

## Que hace ahora mismo

- compila una app Qt para macOS
- lee el token de Cesium ion desde `cesium.conf`
- abre `CesiumJS` dentro de la ventana con `QWebEngineView`
- carga `CesiumJS` desde una copia local en `vendor/cesiumjs`
- carga el globo 3D usando servicios online de Cesium ion

## Requisitos

- macOS
- CMake `>= 3.21`
- compilador C++ con soporte C++17
- Qt 6.10.2 con estos modulos:
  `Widgets`, `Gui`, `WebEngineWidgets`, `WebEngineCore`, `WebChannel`, `Positioning`

En este proyecto se ha usado Qt en:

```bash
/Users/angelconde/Qt/6.10.2/macos
```

Si tu instalacion de Qt esta en otra ruta, cambia `CMAKE_PREFIX_PATH`.

## Configuracion de Cesium ion

La app lee `cesium.conf` en la raiz del repo.

Ejemplo:

```ini
ion_access_token='TU_TOKEN'
ion_asset_id=1
```

Campos:

- `ion_access_token`: token de Cesium ion
- `ion_asset_id`: id numerico mostrado por la UI

## Configurar el proyecto

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=/Users/angelconde/Qt/6.10.2/macos
```

## Compilar

```bash
cmake --build build --target qttest -- -j4
```

## Ejecutar

```bash
./build/qttest.app/Contents/MacOS/qttest
```

## Usar VS Code

El repo incluye estas configuraciones:

- `.vscode/tasks.json`
- `.vscode/launch.json`

Flujo recomendado:

1. Lanzar `Launch qttest`

La tarea de build ya ejecuta antes `CMake Configure`, asi que no hace falta hacerlo a mano desde VS Code.

## Qt Designer

Puedes abrir directamente:

- `src/MainWindow.ui`

Ese archivo contiene la estructura visual principal de la ventana. El visor de `CesiumJS` se inserta despues en tiempo de ejecucion dentro de `viewerHost`.
