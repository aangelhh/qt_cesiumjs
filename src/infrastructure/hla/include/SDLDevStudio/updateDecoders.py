#!/usr/bin/env python3
"""
updateDecoders.py

- Recolecta Encoder headers y headers base (src/inc)
- Extrae el tipo real desde operator<< en cada header
- Genera includes, miembros, declaraciones y definiciones
- Reemplaza bloques entre marcadores en Decoders.h / Decoders.cpp

Ajusta las rutas al principio si necesitas cambiarlas.
"""
from pathlib import Path
import re
import shutil
import sys

# -----------------------------
# Configuración (ajusta si hace falta)
# -----------------------------
SCRIPT_DIR = Path(__file__).resolve().parent
HEADER_FILE = SCRIPT_DIR / "Decoders.h"
CPP_FILE = SCRIPT_DIR / "Decoders.cpp"

SRC_DIR = (SCRIPT_DIR / "../../../libraryHLA/src/DevStudio/datatypes").resolve()
INC_DIR = (SCRIPT_DIR / "../../../libraryHLA/include/DevStudio/datatypes").resolve()

# Marcadores
INCLUDES_BEGIN = "// === AUTO-GENERATED INCLUDES BEGIN ==="
INCLUDES_END   = "// === AUTO-GENERATED INCLUDES END ==="
MEMBERS_BEGIN  = "// === AUTO-GENERATED MEMBERS BEGIN ==="
MEMBERS_END    = "// === AUTO-GENERATED MEMBERS END ==="
FUNCS_BEGIN    = "// === AUTO-GENERATED FUNCTIONS BEGIN ==="
FUNCS_END      = "// === AUTO-GENERATED FUNCTIONS END ==="
BUILDER_BEGIN  = "// === AUTO-GENERATED BUILDER BEGIN ==="
BUILDER_END    = "// === AUTO-GENERATED BUILDER END ==="

# Excluir patrones
EXCLUDE_PATTERN = re.compile(r'^(Byte|Char|Base)Encoder|CharPair\.h$', re.IGNORECASE)

# -----------------------------
# Helpers
# -----------------------------
def to_param_name(t: str) -> str:
    """
    Convierte la primera letra a minúscula y reemplaza 'Encoder' por 'Decoder' al final si existe.
    Ejemplo: 'EventIdentifierEncoder' -> 'eventIdentifierDecoder'
    """
    if not t:
        return t
    # Primera letra minúscula
    t = t[0].lower() + t[1:]
    # Reemplazar 'Encoder' por 'Decoder' al final
    if t.endswith("Encoder"):
        t = t[:-len("Encoder")] + "Decoder"
    return t

def replace_block(file_path: Path, begin: str, end: str, new_lines):
    """
    Reemplaza el bloque entre la línea que contiene 'begin' y la que contiene 'end'
    por new_lines (lista de strings). Similar al replace_block del script Bash.
    Retorna True si reemplazó, False si no encontró marcadores.
    Hace copia de seguridad .bak del archivo original.
    """
    text = file_path.read_text(encoding="utf-8")
    if begin not in text or end not in text:
        return False
    # backup
    bak = file_path.with_suffix(file_path.suffix + ".bak")
    shutil.copyfile(file_path, bak)
    # construir nuevo contenido
    before, _, rest = text.partition(begin)
    _, _, after = rest.partition(end)
    new_block = begin + "\n" + "\n".join(new_lines) + "\n" + end
    new_text = before + new_block + after
    file_path.write_text(new_text, encoding="utf-8")
    if bak.exists():
        bak.unlink()
    return True

def extract_type_from_header(header_path: Path) -> str:
    """
    Extrae el tipo usado en operator<< incluso si:
      - tiene plantillas (< >)
      - tiene namespaces (::)
      - usa const TYPE& o TYPE const&
      - está en std::ostream o std::wostream
    """
    txt = header_path.read_text(encoding="utf-8", errors="ignore")

    # Limpieza de comentarios
    txt_clean = re.sub(r'/\*.*?\*/', '', txt, flags=re.DOTALL)
    txt_clean = re.sub(r'//.*', '', txt_clean)

    # Tipo complejo: template-safe (<...>), namespaces, etc.
    type_pattern = r'[\w:\s]+(?:<[^<>]*?(?:<[^<>]*?>[^<>]*?)*>)?'

    # Regex robusta que captura operator<< con const TYPE& o TYPE const&
    pattern = re.compile(
        rf'operator\s*<<\s*\([^,]+,[^)]*?'
        rf'(?:const\s+({type_pattern})|({type_pattern})\s+const)\s*&',
        re.DOTALL
    )

    m = pattern.search(txt_clean)
    if m:
        typ = m.group(1) or m.group(2)
        return re.sub(r'\s+', ' ', typ.strip())  # normaliza espacios

    return ""

# -----------------------------
# Recolectar headers
# -----------------------------
all_headers = []   # list of tuples (filename, fullpath)
has_encoder = {}   # base_typ -> True

# Buscar Encoder headers en SRC_DIR
if SRC_DIR.exists():
    for p in sorted(SRC_DIR.glob("*Encoder.h")):
        fname = p.name
        all_headers.append((fname, str(p)))
        base_typ = fname[:-len(".h")]
        # quitar sufijo Encoder
        if base_typ.endswith("Encoder"):
            base = base_typ[:-len("Encoder")]
            has_encoder[base] = True

# Añadir .h base desde SRC_DIR y INC_DIR solo si existe encoder
for dir_path in (SRC_DIR, INC_DIR):
    if not dir_path.exists():
        continue
    for p in sorted(dir_path.glob("*.h")):
        fname = p.name
        if EXCLUDE_PATTERN.match(fname):
            continue
        base = fname[:-len(".h")]
        if has_encoder.get(base):
            all_headers.append((fname, str(p)))

# Deduplicate preserving order
seen = set()
unique_headers = []
for fname, full in all_headers:
    if fname in seen:
        continue
    seen.add(fname)
    unique_headers.append((fname, full))
all_headers = unique_headers

# -----------------------------
# Generar includes y miembros
# -----------------------------
includes_lines = []
members_lines = []
seen_includes = set()
seen_members = set()
has_encoder_map = {}  # base -> True (recompute from list)

# recompute has_encoder_map from header files (Encoder.h present)
for fname, _ in all_headers:
    if fname.endswith("Encoder.h"):
        typ = fname[:-len(".h")]
        base = typ[:-len("Encoder")] if typ.endswith("Encoder") else typ
        has_encoder_map[base] = True

# Generate includes and members for entries that are encoder headers
for fname, fullpath in all_headers:
    if not fname.endswith("Encoder.h"):
        continue
    typ = fname[:-len(".h")]  # e.g. AccelerationVectorStructEncoder
    base = typ[:-len("Encoder")] if typ.endswith("Encoder") else typ

    # include encoder header
    inc_line = f'#include <DevStudio/datatypes/{typ}.h>'
    if inc_line not in seen_includes:
        includes_lines.append(inc_line)
        seen_includes.add(inc_line)

    # if base header exists in the list, include it too
    base_header = f'{base}.h'
    if any(hn == base_header for hn, _ in all_headers):
        inc_base = f'#include <DevStudio/datatypes/{base}.h>'
        if inc_base not in seen_includes:
            includes_lines.append(inc_base)
            seen_includes.add(inc_base)

    # member line
    param = to_param_name(typ)
    member_line = f'        {typ} _{param};'
    if member_line not in seen_members:
        members_lines.append(member_line)
        seen_members.add(member_line)

# Ordenar includes alfabéticamente
includes_lines = sorted(includes_lines)

# -----------------------------
# Generar funciones (declaraciones) en Encoders.h
# -----------------------------
func_lines = []
for fname, fullpath in all_headers:
    if fname.endswith("Encoder.h"):
        continue
    typ = fname[:-len(".h")]
    pname = to_param_name(typ)
    real_type = extract_type_from_header(Path(fullpath))
    if not real_type:
        print(f"WARNING: No operator<< found or couldn't extract type in {fname}", file=sys.stderr)
        continue
    decl = f"        {real_type} decode{typ}(const std::vector<char>& data);"
    func_lines.append(decl)

# -----------------------------
# Generar constructor builder (Decoders::Decoders() :)
# -----------------------------
builder_lines = []
builder_lines.append("    Decoders::Decoders() :")
# Need names of variables from members_lines, remove leading spaces and trailing ;
vars_list = []
for m in members_lines:
    # m like "        Type _typeEncoder;"
    s = m.strip()
    if not s:
        continue
    # split into type and var
    parts = s.split()
    if len(parts) >= 2:
        var = parts[-1].rstrip(';')
        vars_list.append(var)

# create initializer list entries
for i, var in enumerate(vars_list):
    comma = "," if i < len(vars_list) - 1 else ""
    builder_lines.append(f"        {var}(){comma}")

# -----------------------------
# Generar definiciones de funciones en Encoders.cpp
# -----------------------------
cpp_func_defs = []
for fname, fullpath in all_headers:
    if fname.endswith("Encoder.h"):
        continue
    typ = fname[:-len(".h")]
    pname = to_param_name(typ)
    real_type = extract_type_from_header(Path(fullpath))
    if not real_type:
        # skip, already warned
        continue
    var_name = f"_{pname}Decoder"
    cpp_func_defs.append(f"    {real_type} Decoders::decode{typ}(const std::vector<char>& data)")
    cpp_func_defs.append("    {")
    cpp_func_defs.append("        std::vector<char> buffer;")
    cpp_func_defs.append("        std::unique_lock<std::mutex> lock(_decodersLock);")
    cpp_func_defs.append(f"        {var_name}.decodeFrom(data, 0);")
    cpp_func_defs.append(f"        buffer.reserve({var_name}.getEncodedLength());")
    cpp_func_defs.append(f"        {var_name}.encodeInto(buffer);")
    cpp_func_defs.append(f"        return {real_type}({var_name});")
    cpp_func_defs.append("    }")

# -----------------------------
# Reemplazar bloques en Encoders.h y Encoders.cpp
# -----------------------------
# Backup original files
if not HEADER_FILE.exists():
    print(f"ERROR: {HEADER_FILE} no existe", file=sys.stderr)
    sys.exit(1)
if not CPP_FILE.exists():
    print(f"ERROR: {CPP_FILE} no existe", file=sys.stderr)
    sys.exit(1)

# Replace includes
ok = replace_block(HEADER_FILE, INCLUDES_BEGIN, INCLUDES_END, includes_lines)
if not ok:
    print(f"WARNING: No se reemplazó includes en {HEADER_FILE} (marcadores no encontrados)", file=sys.stderr)
# Replace members
ok = replace_block(HEADER_FILE, MEMBERS_BEGIN, MEMBERS_END, members_lines)
if not ok:
    print(f"WARNING: No se reemplazó members en {HEADER_FILE} (marcadores no encontrados)", file=sys.stderr)
# Replace function declarations
ok = replace_block(HEADER_FILE, FUNCS_BEGIN, FUNCS_END, func_lines)
if not ok:
    print(f"WARNING: No se reemplazó funciones en {HEADER_FILE} (marcadores no encontrados)", file=sys.stderr)

# Replace builder in CPP
ok = replace_block(CPP_FILE, BUILDER_BEGIN, BUILDER_END, builder_lines)
if not ok:
    print(f"WARNING: No se reemplazó builder en {CPP_FILE} (marcadores no encontrados)", file=sys.stderr)

# Replace function bodies in CPP
ok = replace_block(CPP_FILE, FUNCS_BEGIN, FUNCS_END, cpp_func_defs)
if not ok:
    print(f"WARNING: No se reemplazó funciones en {CPP_FILE} (marcadores no encontrados)", file=sys.stderr)

print("✅ Decoders.h / Decoders.cpp actualizados (si se encontraron marcadores).")
