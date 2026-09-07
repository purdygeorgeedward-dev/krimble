# Krimble — Roadmap Notes

Status: **planning only** — not started. Captured here so they don't stay stuck in chat.

## Advanced JPEG Export Module

Goal: selective/regional compression instead of one global quality slider — get
files that look uncompressed at meaningfully smaller sizes, the way tools like
Guetzli, MozJPEG (trellis quantization), and XAT-style optimizers do.

**Core mechanism:** build a perceptual quality map across the image instead of
applying one quality level everywhere:

- **Edge/detail-heavy regions** (text, logos, sharp transitions) -> higher
  quality/lower compression -- artifacts are most visible here.
- **Flat/gradient regions** (backgrounds, sky, smooth fills) -> deceptive:
  hide detail loss well, but reveal banding easily, so these need careful
  handling rather than just "compress harder because there's nothing there."
- **Texture-heavy/busy regions** -> most tolerant of aggressive compression.

Quality map can be built from Sobel/Laplacian edge detection or local
variance per block; ideally validated with a perceptual metric (SSIM or
butteraugli) comparing candidate compression levels against the source and
picking the lowest quality that stays under a visible-difference threshold,
per region.

**Grayscale/B&W-specific behavior:** B&W images should get a more aggressive
default compression curve than color images. JPEG normally spends a large
share of its bit budget on chroma subsampling, which a grayscale image
doesn't need at all -- every bit goes to luminance/edge fidelity instead.
Combined with the regional map above, flat B&W regions (line art, text,
high-contrast logo/mascot work) can take much heavier quantization than
their color equivalents before anything perceptible changes.

**Implementation notes:** likely built on libjpeg-turbo, either via per-region
quantization table manipulation if exposed directly, or a tile-based
export-and-reassemble approach if not.

## New Tools

- **Smudge tool** -- ✅ shipped (commit `7390d6b`). Dedicated toolbox tool
  wrapping Krita's existing colorsmudge paintop engine, own icon/shortcut/
  toolbox slot, auto-loads the "smudge" preset on activation.
- **Soften tool** -- not started. Photoshop's Blur tool equivalent: a
  localized blur/softening brush for softening edges/skin/detail in specific
  areas without running a full-image filter. Krita's `filterop` paintop
  engine (paint-with-any-filter) already exists and can drive this --
  the work is presetting it to Gaussian Blur with sane defaults and giving
  it its own toolbox entry, same pattern as Smudge.
