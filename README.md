# 2far melodies — autotune VST3 (JUCE / C++)

Autotune en rosa con interfaz estilo revista anime ("Something Weird"), todo el texto en fuente pixel propia.
Créditos "DEV BY ELYJA4" en la esquinita, junto al árbol de cerezo (cambia el texto en `Source/Backdrop.h`, constante `credit`).

## Cómo funciona (audio real)
1. **Detección de tono** (algoritmo tipo YIN, 60–1000 Hz) sobre la mezcla mono, cada ~11.6 ms.
2. **Cuantización** a la tonalidad y escala elegidas (Cromática, Mayor, Menor, Pentatónica Mayor/Menor), con histéresis.
3. **Corrección** suavizada: SPEED = 0 ms es afinado duro (efecto "robot"); valores altos suenan más naturales.
4. **Cambio de tono** con dos líneas de retardo y fundido, con ventana síncrona al periodo de la voz.
Latencia fija aprox. 12.5 ms informada al host.

## Controles (automatizables)
KEY, SCALE, SPEED (0–400 ms), AMOUNT (0–100 %), MIX (0–100 %).

## Interfaz
- Medidor de notas: nota detectada (grande), objetivo, cents, historial de afinación, y 12 notas con letras rosas (las que no son de la escala se apagan).
- Cada vez que cambia la nota, caen hojas/pétalos de cerezo en píxeles desde el árbol.

## Compilar en Windows
```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```
Resultado: `build\Melodies_artefacts\Release\VST3\2far melodies.vst3`
Instalar: copiar la carpeta a `C:\Program Files\Common Files\VST3\` y buscar plugins en FL Studio.
(Con GitHub Actions: `.github/workflows/build.yml`; también ejecuta `tests/test_pitch.cpp`.)

## Limitaciones conocidas
- Funciona para voz/instrumento monofónico. En audio polifónico no hay corrección fiable.
- El cambio de tono es de calidad básica (sin preservación de formantes): correcciones grandes suenan más artificiales.
- AU solo se compila en macOS.
