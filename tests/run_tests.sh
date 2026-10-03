set -u
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
EXE="asm_test.exe"

echo "Building asm.c..."
gcc -std=c99 -Wall -Wextra -g "$PROJECT_DIR/src"/*.c -o "$SCRIPT_DIR/$EXE" 2>"$SCRIPT_DIR/.build.log"
if [ ! -f "$SCRIPT_DIR/$EXE" ]; then
    echo "BUILD FAILED, see tests/.build.log"
    cat "$SCRIPT_DIR/.build.log"
    exit 1
fi

cd "$SCRIPT_DIR" || exit 1

pass=0
fail=0

for asm_file in *.asm; do
    name="${asm_file%.asm}"
    expected_file="$name.expected"
    if [ ! -f "$expected_file" ]; then
        continue
    fi
    actual="$(./"$EXE" -c -f "$asm_file" 2>&1 | tr -d '\r')"
    expected="$(cat "$expected_file" | tr -d '\r')"
    if [ "$actual" == "$expected" ]; then
        echo "PASS: $name"
        pass=$((pass+1))
    else
        echo "FAIL: $name"
        echo "  --- expected ---"
        echo "$expected" | sed 's/^/  /'
        echo "  --- actual ---"
        echo "$actual" | sed 's/^/  /'
        fail=$((fail+1))
    fi
done

echo ""
echo "$pass passed, $fail failed"
rm -f "$SCRIPT_DIR/$EXE"
[ "$fail" -eq 0 ]
