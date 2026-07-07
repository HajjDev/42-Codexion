#!/usr/bin/env bash
# ============================================================
# test_codexion.sh
# Suite de tests pour le projet "codexion" (42 - variant Philosophers)
# Usage : ./test_codexion.sh /chemin/vers/codexion
# Si aucun chemin n'est donné, cherche ./codexion
# ============================================================

set -u

BIN="${1:-./codexion}"
PASS=0
FAIL=0
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_ok()   { echo -e "${GREEN}[OK]${NC} $1"; PASS=$((PASS+1)); }
log_fail() { echo -e "${RED}[FAIL]${NC} $1"; FAIL=$((FAIL+1)); }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
section()  { echo -e "\n=== $1 ==="; }

if [ ! -x "$BIN" ]; then
    echo "Binaire introuvable ou non exécutable : $BIN"
    echo "Usage: $0 /chemin/vers/codexion"
    exit 1
fi

now_ms() { echo $(($(date +%s%N)/1000000)); }

# ------------------------------------------------------------
# 1. PARSING - arguments invalides : doit rejeter proprement,
#    exit code != 0, PAS de segfault, PAS de sortie infinie.
# ------------------------------------------------------------
section "1. Parsing des arguments invalides"

run_should_reject() {
    local desc="$1"; shift
    timeout 2 "$BIN" "$@" > "$TMPDIR/out" 2>"$TMPDIR/err"
    local code=$?
    if [ "$code" -eq 124 ]; then
        log_fail "$desc -> timeout (bloqué au lieu de rejeter)"
    elif [ "$code" -eq 139 ] || [ "$code" -eq 134 ]; then
        log_fail "$desc -> CRASH (segfault/abort, code=$code)"
    elif [ "$code" -eq 0 ]; then
        log_fail "$desc -> accepté alors que ça devrait être rejeté (code=0)"
    else
        log_ok "$desc -> rejeté proprement (code=$code)"
    fi
}

run_should_reject "0 coder"                 0 4000 200 200 200 3 100 fifo
run_should_reject "coder negatif"           -1 4000 200 200 200 3 100 fifo
run_should_reject "burnout negatif"         3 -4000 200 200 200 3 100 fifo
run_should_reject "non entier (abc)"        abc 4000 200 200 200 3 100 fifo
run_should_reject "float (12.5)"            12.5 4000 200 200 200 3 100 fifo
run_should_reject "notation scientifique"   3 1e3 200 200 200 3 100 fifo
run_should_reject "string vide"             "" 4000 200 200 200 3 100 fifo
run_should_reject "scheduler invalide"      3 4000 200 200 200 3 100 FIFO
run_should_reject "scheduler avec espace"   3 4000 200 200 200 3 100 "edf "
run_should_reject "scheduler bidon"         3 4000 200 200 200 3 100 edf2
run_should_reject "overflow (trop grand)"   3 99999999999999999999 200 200 200 3 100 fifo
run_should_reject "trop peu d'arguments"    3 4000 200 200
run_should_reject "trop d'arguments"        3 4000 200 200 200 3 100 fifo extra
run_should_reject "argument avec espaces"   3 4000 200 200 200 3 100 " fifo"
run_should_reject "zero argument"

echo "--- Cas limites qui DOIVENT être acceptés (vérifie à la main que ça tourne) ---"
timeout 1 "$BIN" 3 4000 200 200 200 0 100 fifo > "$TMPDIR/zero_req" 2>&1
if [ $? -eq 124 ]; then
    log_warn "number_of_compiles_required=0 : le programme tourne encore après 1s, il devrait s'arrêter immédiatement"
else
    log_ok "number_of_compiles_required=0 : s'arrête sans tourner indéfiniment"
fi

# ------------------------------------------------------------
# 2. CAS number_of_coders = 1 : un seul dongle sur la table,
#    burnout garanti (piège classique du sujet).
# ------------------------------------------------------------
section "2. number_of_coders = 1 (burnout garanti)"

BURNOUT_MS=1000
START=$(now_ms)
timeout 5 "$BIN" 1 "$BURNOUT_MS" 200 200 200 5 100 fifo > "$TMPDIR/single" 2>&1
END=$(now_ms)
ELAPSED=$((END-START))

if grep -q "compiling" "$TMPDIR/single"; then
    log_fail "coder unique : ne devrait jamais réussir à compiler (1 seul dongle disponible)"
else
    log_ok "coder unique : ne compile jamais (comportement attendu)"
fi

if grep -q "burned out" "$TMPDIR/single"; then
    log_ok "coder unique : burnout détecté"
    DIFF=$((ELAPSED - BURNOUT_MS))
    ABS_DIFF=${DIFF#-}
    if [ "$ABS_DIFF" -le 60 ]; then
        log_ok "coder unique : burnout survenu ~${ELAPSED}ms (attendu ~${BURNOUT_MS}ms, delta ${DIFF}ms)"
    else
        log_warn "coder unique : timing suspect, elapsed=${ELAPSED}ms attendu=${BURNOUT_MS}ms (delta ${DIFF}ms)"
    fi
else
    log_fail "coder unique : AUCUN burnout loggé alors qu'il aurait dû se produire"
fi

# ------------------------------------------------------------
# 3. FORMAT DES LOGS - chaque ligne doit matcher exactement
#    le format attendu, aucune ligne mélangée.
# ------------------------------------------------------------
section "3. Format des logs (run normal, 4 coders)"

timeout 5 "$BIN" 4 3000 200 200 200 3 50 fifo > "$TMPDIR/fmt" 2>&1
PATTERN='^[0-9]+ [0-9]+ (has taken a dongle|is compiling|is debugging|is refactoring|burned out)$'
BAD_LINES=0
LINE_COUNT=0
while IFS= read -r line; do
    LINE_COUNT=$((LINE_COUNT+1))
    if ! echo "$line" | grep -qE "$PATTERN"; then
        BAD_LINES=$((BAD_LINES+1))
        echo "  ligne suspecte: $line"
    fi
done < "$TMPDIR/fmt"

if [ "$LINE_COUNT" -eq 0 ]; then
    log_fail "format des logs : aucune ligne produite"
elif [ "$BAD_LINES" -eq 0 ]; then
    log_ok "format des logs : $LINE_COUNT lignes, toutes conformes"
else
    log_fail "format des logs : $BAD_LINES/$LINE_COUNT lignes non conformes (entrelacement possible ?)"
fi

# Vérifie que chaque "compiling" est précédé de 2 "has taken a dongle" pour le même coder
awk '
{
    ts=$1; id=$2; $1=""; $2=""; action=$0; sub(/^  /,"",action)
    if (action == "has taken a dongle") dongles[id]++
    if (action == "is compiling") {
        if (dongles[id] < 2) { print "coder " id " compile avec " dongles[id] " dongle(s) seulement (ligne: " $0 ")"; bad=1 }
        dongles[id]=0
    }
}
END { if (bad) exit 1; else exit 0 }
' "$TMPDIR/fmt"
if [ $? -eq 0 ]; then
    log_ok "cohérence dongles->compile : chaque compile est précédé de 2 prises de dongle"
else
    log_fail "cohérence dongles->compile : un coder compile sans avoir 2 dongles"
fi

# Vérifie que les timestamps sont croissants (non strictement, égalité OK)
awk '{ if ($1 < prev) { print "timestamp qui recule: " $0; bad=1 }; prev=$1 } END { exit bad }' "$TMPDIR/fmt"
if [ $? -eq 0 ]; then
    log_ok "timestamps monotones (jamais de retour en arrière)"
else
    log_fail "timestamps non monotones détectés"
fi

# ------------------------------------------------------------
# 4. STRESS TEST - beaucoup de coders, pas de crash, pas de fuite
#    évidente, pas d'entrelacement de logs.
# ------------------------------------------------------------
section "4. Stress test (200 coders)"

timeout 8 "$BIN" 200 5000 200 200 200 2 50 edf > "$TMPDIR/stress" 2>"$TMPDIR/stress_err"
CODE=$?
if [ "$CODE" -eq 139 ] || [ "$CODE" -eq 134 ]; then
    log_fail "stress test 200 coders : CRASH (code=$CODE)"
elif [ "$CODE" -eq 124 ]; then
    log_warn "stress test 200 coders : timeout après 8s (peut être normal selon params, à vérifier à la main)"
else
    log_ok "stress test 200 coders : pas de crash (code=$CODE)"
fi
NB_LINES=$(wc -l < "$TMPDIR/stress")
log_warn "stress test : $NB_LINES lignes produites, vérifie à l'oeil qu'aucune n'est tronquée/fusionnée"

# ------------------------------------------------------------
# 5. TIE-BREAK EDF déterminisme : deux runs identiques doivent
#    donner le même ordre de service (déterminisme du scheduler)
# ------------------------------------------------------------
section "5. Déterminisme EDF (2 runs identiques)"

timeout 4 "$BIN" 5 4000 200 200 200 2 50 edf > "$TMPDIR/edf1" 2>&1
timeout 4 "$BIN" 5 4000 200 200 200 2 50 edf > "$TMPDIR/edf2" 2>&1
ORDER1=$(grep "is compiling" "$TMPDIR/edf1" | awk '{print $2}' | tr '\n' ' ')
ORDER2=$(grep "is compiling" "$TMPDIR/edf2" | awk '{print $2}' | tr '\n' ' ')
if [ "$ORDER1" == "$ORDER2" ]; then
    log_ok "EDF déterministe : même ordre de compile sur 2 runs ($ORDER1)"
else
    log_warn "EDF : ordres différents entre 2 runs (peut être normal à cause du timing réel, à examiner)"
    echo "  run1: $ORDER1"
    echo "  run2: $ORDER2"
fi

# ------------------------------------------------------------
# 6. VALGRIND - fuites mémoire et data races (si installé)
# ------------------------------------------------------------
section "6. Valgrind (memcheck + helgrind si disponibles)"

if command -v valgrind >/dev/null 2>&1; then
    timeout 10 valgrind --leak-check=full --error-exitcode=42 \
        "$BIN" 3 3000 200 200 200 2 50 fifo > "$TMPDIR/vg_out" 2>"$TMPDIR/vg_err"
    NO_ERRORS=0
    NO_LEAKS=0
    grep -qE "ERROR SUMMARY: 0 errors" "$TMPDIR/vg_err" && NO_ERRORS=1
    if grep -qE "All heap blocks were freed -- no leaks are possible" "$TMPDIR/vg_err"; then
        NO_LEAKS=1
    elif grep -qE "definitely lost: 0 bytes" "$TMPDIR/vg_err" && ! grep -qE "indirectly lost: [1-9]" "$TMPDIR/vg_err"; then
        NO_LEAKS=1
    fi
    if [ "$NO_ERRORS" -eq 1 ] && [ "$NO_LEAKS" -eq 1 ]; then
        log_ok "valgrind memcheck : pas de fuite ni d'erreur détectée"
    else
        log_fail "valgrind memcheck : fuite(s) ou erreur(s) détectée(s), voir $TMPDIR/vg_err"
        grep -E "definitely lost|indirectly lost|ERROR SUMMARY|All heap blocks" "$TMPDIR/vg_err"
    fi

    if valgrind --tool=helgrind --version >/dev/null 2>&1; then
        timeout 10 valgrind --tool=helgrind "$BIN" 3 3000 200 200 200 2 50 fifo > "$TMPDIR/hg_out" 2>"$TMPDIR/hg_err"
        if grep -qE "ERROR SUMMARY: 0 errors" "$TMPDIR/hg_err"; then
            log_ok "helgrind : pas de data race détectée"
        else
            log_fail "helgrind : data race(s) potentielle(s), voir $TMPDIR/hg_err"
            grep -A6 "Possible data race" "$TMPDIR/hg_err" | head -40
        fi
    fi
else
    log_warn "valgrind non installé, tests mémoire/data-race sautés (installe-le: apt install valgrind)"
fi

# ------------------------------------------------------------
# 7. Sortie propre sur burnout (pas de logs après l'arrêt)
# ------------------------------------------------------------
section "7. Pas de logs après l'arrêt de la simulation"

# time_to_burnout (400) < compile+debug+refactor (600) => burnout garanti dès le 1er cycle,
# indépendamment de la contention sur les dongles. Rend ce test déterministe.
timeout 5 "$BIN" 3 400 200 200 200 10 50 fifo > "$TMPDIR/afterstop" 2>&1
STOP_LINE=$(grep -n "burned out" "$TMPDIR/afterstop" | head -1 | cut -d: -f1)
TOTAL_LINES=$(wc -l < "$TMPDIR/afterstop")
if [ -n "$STOP_LINE" ]; then
    if [ "$STOP_LINE" -eq "$TOTAL_LINES" ]; then
        log_ok "aucune ligne après le burnout"
    else
        log_fail "$((TOTAL_LINES - STOP_LINE)) ligne(s) loggée(s) après le burnout (race à l'arrêt ?)"
        sed -n "$((STOP_LINE+1)),\$p" "$TMPDIR/afterstop"
    fi
else
    log_fail "burnout attendu (params garantissent time_to_burnout < cycle complet) mais AUCUN burnout loggé"
fi

# ------------------------------------------------------------
# RÉSUMÉ
# ------------------------------------------------------------
section "RÉSUMÉ"
echo -e "${GREEN}$PASS tests réussis${NC} / ${RED}$FAIL tests échoués${NC}"
echo "Logs détaillés conservés dans: $TMPDIR (sera supprimé à la fin du script si tu ne commentes pas le trap)"
[ "$FAIL" -eq 0 ] && exit 0 || exit 1