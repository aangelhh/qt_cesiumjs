# qttest

Visor base en C++ con Qt 6 que carga `CesiumJS` dentro de `Qt WebEngine`.

## Qué hace ahora mismo

- Compila una app Qt para macOS y Linux.
- Lee el token de Cesium ion desde `cesium.conf`.
- Abre `CesiumJS` dentro de la ventana con `QWebEngineView`.
- Carga `CesiumJS` desde una copia local en `vendor/cesiumjs`.
- Carga el globo 3D usando servicios online de Cesium ion.

## Requisitos

- macOS o Linux (Ubuntu/Debian recomendado)
- CMake `>= 3.21`
- Compilador C++ con soporte C++17
- Qt 6 con los módulos:
  `Widgets`, `Gui`, `WebEngineWidgets`, `WebEngineCore`, `WebChannel`, `Positioning`

### Instalación de Dependencias

Se incluyen scripts automáticos para preparar el entorno:

**Para Linux (Debian/Ubuntu):**
```bash
./scripts/install_deps_linux.sh
```

**Para macOS (requiere Homebrew):**
```bash
./scripts/install_deps_macos.sh
```

## Configuración de Cesium ion

La app lee `cesium.conf` en la raíz del repo.

Ejemplo:

```ini
ion_access_token='TU_TOKEN'
ion_asset_id=1
```

Campos:

- `ion_access_token`: token de Cesium ion
- `ion_asset_id`: id numérico mostrado por la UI

## Compilar

Usa el script de construcción que autodetecta tu plataforma y el path de Qt:

```bash
cd qttest
./build.sh
```

Si deseas compilar a mano, asegúrate de exportar `CMAKE_PREFIX_PATH` apuntando a tu instalación de Qt6. Por ejemplo:

```bash
cd qttest
mkdir -p build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/ruta/a/tu/qt6
cmake --build . -j4
```

## Ejecutar

**En macOS:**
```bash
./qttest/build/qttest.app/Contents/MacOS/qttest
```

**En Linux:**
```bash
./qttest/build/qttest
```

## Usar VS Code

El repo incluye estas configuraciones:

- `.vscode/tasks.json`
- `.vscode/launch.json`

Flujo recomendado:

1. Lanzar `Launch qttest`

La tarea de build ya ejecuta antes `CMake Configure`, así que no hace falta hacerlo a mano desde VS Code.

## Qt Designer

Puedes abrir directamente:

- `src/MainWindow.ui`

Ese archivo contiene la estructura visual principal de la ventana. El visor de `CesiumJS` se inserta después en tiempo de ejecución dentro de `viewerHost`.
