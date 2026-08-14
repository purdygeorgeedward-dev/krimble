project(kritafilters)

add_subdirectory(libkra)

if(JPEG_FOUND AND HAVE_LCMS2)
	add_subdirectory(jpeg)
endif()

if(TIFF_FOUND AND JPEG_FOUND)
	add_subdirectory(tiff)
endif()

if(PNG_FOUND)
    add_subdirectory(png)
    # add_subdirectory(csv)   # animation export, not an image format
endif()

# if(OpenEXR_FOUND)
#     add_subdirectory(exr)    # VFX/compositing format, niche
# endif()

if(Poppler_Qt${QT_MAJOR_VERSION}_FOUND)
    add_subdirectory(pdf)
endif()

if(OpenJPEG_FOUND AND NOT ${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    add_subdirectory(jp2)
endif()

if(KF5KDcraw_FOUND OR KDcrawQt6_FOUND)
    add_subdirectory(raw)
endif()

add_subdirectory(svg)
add_subdirectory(qimageio)
add_subdirectory(ora)
add_subdirectory(xcf)
add_subdirectory(psd)
# add_subdirectory(qml)        # not an image format, no reason to expose it
add_subdirectory(tga)
# add_subdirectory(heightmap)  # 3D terrain data, niche
# add_subdirectory(brush)      # painting brush presets, out of scope for Krimble
# add_subdirectory(spriter)    # game sprite animation, niche
add_subdirectory(kra)
if (GIF_FOUND)
    add_subdirectory(gif)
endif()

if (HEIF_FOUND)
    add_subdirectory(heif)
endif()

add_subdirectory(krz)

if (WebP_FOUND)
    add_subdirectory(webp)
endif()

if (JPEGXL_FOUND)
    add_subdirectory(jxl)
endif()

# add_subdirectory(rgbe)      # HDR radiance images, VFX-only
