#! /bin/bash

set -euo pipefail

BASE_PATH="$(dirname "$0")/../../.."
cd "$BASE_PATH"

INPUT="$1"
shift 1

RUNTIME_SRC="src/main/resources/src/main/java/ar/edu/itba/atlyc"
OUTPUT_JAVA="output/src/main/java/ar/edu/itba/atlyc"

# Compilar el programa → genera output/src/main/java/ar/edu/itba/atlyc/Main.java
cat "$INPUT" | ".build/Flex-Bison-Compiler" "$@"

# Copiar los archivos del runtime (todo excepto Main.java)
for file in "$RUNTIME_SRC"/*.java; do
    filename="$(basename "$file")"
    if [ "$filename" != "Main.java" ]; then
        cp "$file" "$OUTPUT_JAVA/$filename"
    fi
done

# Copiar el pom.xml desde resources al output
cp "src/main/resources/pom.xml" "output/pom.xml"
