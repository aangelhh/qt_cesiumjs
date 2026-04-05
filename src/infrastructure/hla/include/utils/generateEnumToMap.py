#!/usr/bin/env python3
import re
from pathlib import Path

# -----------------------------
# Configuración de rutas
# -----------------------------
folder_path = Path("../../../libraryHLA/include/DevStudio")
hla_object_file = folder_path / "HlaObjectInstanceBase.h"
header_files = list(folder_path.glob("*Attributes.h"))
output_file_cpp = Path("Utils.cpp")

all_maps = []

# -----------------------------
# 1. Extraer ObjectClassType enum
# -----------------------------
with hla_object_file.open(encoding="utf-8") as f:
    content = f.read()

match = re.search(r'enum\s+ObjectClassType\s*{([^}]*)}', content, re.MULTILINE | re.DOTALL)
if match:
    enum_content = match.group(1)
    # Extraer valores (solo nombres)
    enum_values = [m.group(1) for m in re.finditer(r'\b([A-Z_]+)\b', enum_content)]
else:
    raise ValueError("No se encontró el enumerado ObjectClassType")

# Función para normalizar nombres (eliminar no alfanuméricos y pasar a minúsculas)
def normalize_name(name):
    return re.sub(r'[^a-z0-9]', '', name.lower())

enum_norm_map = {normalize_name(val): val for val in enum_values}

# -----------------------------
# 2. Procesar cada archivo *Attributes.h
# -----------------------------
for file in header_files:
    if not file.exists():
        print(f"WARNING: Archivo no encontrado: {file}")
        continue

    content = file.read_text(encoding="utf-8")

    # Limpiar comentarios
    content = re.sub(r'//.*', '', content)
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    content = re.sub(r'\s+', ' ', content)

    # Buscar todos los enums
    for enum_match in re.finditer(r'enum\s+(\w+)\s*\{(.*?)\};', content, re.DOTALL):
        enum_name = enum_match.group(1)
        enum_body = enum_match.group(2)
        base_file_name = file.stem  # nombre del archivo sin extensión

        # Limpiar nombre: quitar "Hla" y "Attributes"
        processed_name = re.sub(r'^Hla', '', base_file_name)
        processed_name = re.sub(r'Attributes$', '', processed_name)

        # Normalizar para comparar
        processed_name_norm = normalize_name(processed_name)

        # Buscar correspondencia en ObjectClassType
        object_class_type = enum_norm_map.get(processed_name_norm)
        if not object_class_type:
            print(f"WARNING: No se encontró {base_file_name} en ObjectClassType")
            continue

        # Construir código del mapa
        map_code = f"{{ DevStudio::HlaObjectInstanceBase::ObjectClassType::{object_class_type}, {{\n"
        auto_value = 0
        for line in enum_body.split(","):
            line = line.strip()
            if not line:
                continue
            m = re.match(r"(\w+)\s*=?\s*(\d*)", line)
            if m:
                name, value = m.groups()
                if not value:
                    value = auto_value
                else:
                    auto_value = int(value)
                map_code += f'    {{"{name}", {value}}},\n'
                auto_value += 1

        map_code += "}},\n"

        # Indentar 4 espacios por el namespace
        indented_code = "\n".join(["    " + l for l in map_code.splitlines()])
        all_maps.append(indented_code)

# -----------------------------
# 3. Reemplazar bloque auto-generado en Utils.cpp
# -----------------------------
content_lines = output_file_cpp.read_text(encoding="utf-8").splitlines()

begin_marker = '// === AUTO-GENERATED objectClassTypeAttributesMap BEGIN ==='
end_marker = '// === AUTO-GENERATED objectClassTypeAttributesMap END ==='

begin_index = next((i for i, line in enumerate(content_lines) if begin_marker in line), -1)
end_index = next((i for i, line in enumerate(content_lines) if end_marker in line), -1)

if begin_index == -1 or end_index == -1:
    raise ValueError("No se encontraron los marcadores correctamente.")

before = content_lines[:begin_index + 1]
after = content_lines[end_index:]

new_content = "\n".join(before + all_maps + after)
output_file_cpp.write_text(new_content, encoding="utf-8")

print(f"Se ha actualizado correctamente {output_file_cpp}")
