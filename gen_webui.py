import pathlib
h = pathlib.Path(__file__).parent / "src"
html = (h / "index.html").read_text(encoding="utf-8")
assert ')rawliteral' not in html
(h / "webui.h").write_text('#pragma once\n// Automatisch erzeugt aus index.html (python3 gen_webui.py)\nconst char INDEX_HTML[] PROGMEM = R"rawliteral(' + html + ')rawliteral";\n', encoding="utf-8")
print("webui.h", len(html), "Bytes")
