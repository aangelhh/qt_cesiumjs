# qt_cesium_native

Base de una aplicacion en C++ con Qt 6 y Cesium Native.

## Que hace ahora mismo

- compila una app Qt nativa
- integra `cesium-native` dentro del proyecto con CMake
- carga configuracion de Cesium ion desde `cesium.conf`
- crea un `Tileset` de ion en runtime
- deja preparada la base para conectar camara y render OpenGL

## Requisitos

- macOS
- CMake `>= 3.21`
- compilador C++ con soporte C++17
- Qt 6 con estos modulos:
  `Widgets`, `OpenGL`, `OpenGLWidgets`, `Gui`

En este proyecto se ha usado Qt en:

```bash
/Users/angelconde/Qt/6.10.2/macos
```

Si tu instalacion de Qt está en otra ruta, cambia `CMAKE_PREFIX_PATH`.

## Configuracion de Cesium ion

La app lee `cesium.conf` en la raiz del repo.

Ejemplo:

```ini
ion_access_token='TU_TOKEN'
ion_asset_id=1
```

Campos:

- `ion_access_token`: token de Cesium ion
- `ion_asset_id`: id numerico del asset que quieres abrir

## Configurar el proyecto

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH=/Users/angelconde/Qt/6.10.2/macos
```

## Compilar

```bash
cmake --build build -- -j1
```

Nota:
en macOS este proyecto se está compilando en modo estable con `-j1` para evitar problemas intermitentes al archivar librerias estaticas de `cesium-native`.

## Ejecutar

```bash
./build/appqt_test.app/Contents/MacOS/appqt_test
```

## Usar VS Code

El repo incluye estas configuraciones:

- [tasks.json](/Users/angelconde/qt_test/.vscode/tasks.json)
- [launch.json](/Users/angelconde/qt_test/.vscode/launch.json)

Flujo recomendado:

1. Ejecutar la tarea `CMake Configure`
2. Ejecutar la tarea `CMake Build`
3. Lanzar `Launch appqt_test`

## Estado actual

La app ya crea el runtime de Cesium Native y puede inicializar un `Tileset` de ion, pero todavia no renderiza geometria 3D en pantalla.

Lo siguiente por implementar es:

- camara
- `updateView()`
- subida de mallas a GPU
- render OpenGL dentro de Qt
