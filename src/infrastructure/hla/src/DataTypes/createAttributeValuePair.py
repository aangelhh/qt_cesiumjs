#!/usr/bin/env python3
import re
from pathlib import Path

# -----------------------------
# Configuración de rutas
# -----------------------------
utils_file = Path("../../include/utils/Utils.cpp")
header_path = Path("../../../libraryHLA/include/DevStudio")
header_files = list(header_path.glob("*Updater.h"))
decoders_file = Path("../../include/SDLDevStudio/Decoders.h")
encoders_file = Path("../../include/SDLDevStudio/Encoders.h")

# -----------------------------
# 1. Extraer atributos de Utils.cpp
# -----------------------------
attribute_set = set()
inside_object = False

with utils_file.open() as f:
    for line in f:
        if "ObjectClassType" in line:
            inside_object = True
            continue
        if inside_object and re.match(r"\s*\}\},?", line):
            inside_object = False
        if inside_object:
            matches = re.findall(r'\{"([^"]+)",', line)
            for m in matches:
                attribute_set.add(m)

attributes = sorted(attribute_set)

# -----------------------------
# 2. Buscar setters en *Updater.h
# -----------------------------
setter_map = {}  # func_name -> type

setter_regex = re.compile(
    r"LIBAPI\s+virtual\s+\w+\s+(set[A-Za-z0-9_]+)\s*\(\s*const\s+([A-Za-z0-9_:<>]+)&"
)

for header in header_files:
    with header.open() as f:
        for line in f:
            m = setter_regex.search(line)
            if m:
                func, arg_type = m.groups()
                # Mantener solo el primer setter encontrado
                if func not in setter_map:
                    setter_map[func] = arg_type.strip()

# -----------------------------
# 3. Cargar decoders y encoders existentes
# -----------------------------
decoder_map = {}
encoder_map = {}

decoder_regex = re.compile(r'([[:alnum:]_:]+)\s+([[:alnum:]_]+)\s*\(.*\)')
with decoders_file.open() as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        m = re.match(r"([A-Za-z0-9_:<>]+)\s+([A-Za-z0-9_]+)\s*\(", line)
        if m:
            ret_type, func_name = m.groups()
            decoder_map[ret_type.strip()] = func_name.strip()

with encoders_file.open() as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        m = re.match(r"std::vector<char>\s+([A-Za-z0-9_]+)\s*\(const\s+([A-Za-z0-9_:<>]+)&", line)
        if m:
            func_name, arg_type = m.groups()
            encoder_map[arg_type.strip()] = func_name.strip()

# -----------------------------
# 4. Generar la tabla
# -----------------------------
def pascal_case(s):
    return "".join(p.capitalize() for p in s.lower().split("_"))

def type_base_name(tp):
    """Quita namespace y vectores para generar nombre de función"""
    m = re.match(r"std::vector<([A-Za-z0-9_:]+)>", tp)
    if m:
        base = m.group(1)
    else:
        base = tp
    return base.split("::")[-1].replace(" ", "")

table_entries = []

for attr in attributes:
    func_name = f"set{pascal_case(attr)}"
    arg_type = setter_map.get(func_name)
    if not arg_type:
        continue

    # Buscar decoder
    decode_func = decoder_map.get(arg_type)
    if not decode_func:
        decode_func = f"decode{type_base_name(arg_type)}"
        if arg_type.startswith("std::vector<"):
            decode_func += "LengthlessArray"

    # Buscar encoder
    encode_func = encoder_map.get(arg_type)
    if not encode_func:
        encode_func = f"encode{type_base_name(arg_type)}"
        if arg_type.startswith("std::vector<"):
            encode_func += "LengthlessArray"

    entry = f"""        {{"{attr}", {{
            [](void*& data, const std::vector<char>& bytes) {{
                auto decoded = Decoders::getInstance().{decode_func}(bytes);
                data = new {arg_type}(decoded);
            }},
            [](void* data) {{
                return Encoders::getInstance().{encode_func}(*static_cast<{arg_type}*>(data));
            }}
        }}}},"""
    table_entries.append(entry)

# -----------------------------
# 5. Escribir resultado
# -----------------------------
cpp_file = Path("AttributeValuePair.cpp")
begin_marker = "// === AUTO-GENERATED BEGIN ==="
end_marker = "// === AUTO-GENERATED END ==="

with cpp_file.open() as f:
    content = f.read()

before, sep, rest = content.partition(begin_marker)
_, sep2, after = rest.partition(end_marker)

new_content = before + begin_marker + "\n" + "\n".join(table_entries) + "\n" + end_marker + after
with cpp_file.open("w") as f:
    f.write(new_content)

print(f"Se ha actualizado correctamente {cpp_file}")
