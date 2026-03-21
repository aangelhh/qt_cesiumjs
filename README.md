# qt_cesium_native

Visor base en C++ con Qt 6 que carga `CesiumJS` dentro de `Qt WebEngine`.

## Que hace ahora mismo

- compila una app Qt para macOS
- lee el token de Cesium ion desde `cesium.conf`
- abre `CesiumJS` dentro de la ventana con `QWebEngineView`
- carga el globo 3D de Cesium online

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
cmake --build build --target appqt_test -- -j4
```

## Ejecutar

```bash
./build/appqt_test.app/Contents/MacOS/appqt_test
```

## Usar VS Code

El repo incluye estas configuraciones:

- [tasks.json](/Users/angelconde/qt_test/.vscode/tasks.json)
- [launch.json](/Users/angelconde/qt_test/.vscode/launch.json)

Flujo recomendado:

1. Lanzar `Launch appqt_test`

La tarea de build ya ejecuta antes `CMake Configure`, asi que no hace falta hacerlo a mano desde VS Code.
