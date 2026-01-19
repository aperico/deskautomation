#!/bin/bash
# Download and extract cppcheck addons (misra.py)
# Usage: ./fetch_cppcheck_addons.sh [target_dir]

TARGET_DIR="${1:-/tmp/cppcheck-addons}"
mkdir -p "$TARGET_DIR"

echo "Fetching cppcheck addons to $TARGET_DIR"

# Try multiple sources in order of preference
URLS=(
  "https://github.com/danmar/cppcheck/archive/refs/tags/2.14.0.zip"
  "https://github.com/danmar/cppcheck/archive/refs/tags/2.13.0.zip"
  "https://github.com/danmar/cppcheck/archive/refs/heads/main.zip"
)

SUCCESS=0
for URL in "${URLS[@]}"; do
  echo "Attempting: $URL"
  
  # Remove any previous failed download
  rm -f "$TARGET_DIR/cppcheck.zip"
  
  # Download with better error handling
  if curl -sL -f --max-time 60 "$URL" -o "$TARGET_DIR/cppcheck.zip"; then
    # Check file exists and has reasonable size
    if [ -f "$TARGET_DIR/cppcheck.zip" ]; then
      FILE_SIZE=$(stat -c%s "$TARGET_DIR/cppcheck.zip" 2>/dev/null || echo "0")
      
      if [ "$FILE_SIZE" -gt 1000000 ]; then
        # Verify zip integrity
        if unzip -t "$TARGET_DIR/cppcheck.zip" > /dev/null 2>&1; then
          echo "✓ Download successful (${FILE_SIZE} bytes)"
          SUCCESS=1
          break
        else
          echo "✗ Zip file corrupted"
        fi
      else
        echo "✗ File too small ($FILE_SIZE bytes)"
      fi
    fi
  else
    echo "✗ Download failed"
  fi
  
  rm -f "$TARGET_DIR/cppcheck.zip"
done

if [ $SUCCESS -eq 0 ]; then
  echo "✗ Failed to download cppcheck addons from all sources"
  exit 1
fi

# Clean old extraction
rm -rf "$TARGET_DIR"/cppcheck-*

# Extract
echo "Extracting addons..."
if ! unzip -q "$TARGET_DIR/cppcheck.zip" -d "$TARGET_DIR"; then
  echo "✗ Extraction failed"
  exit 1
fi

echo "Contents after extraction:"
ls -la "$TARGET_DIR" | head -20

# Find extracted directory
EXTRACTED_DIR=$(find "$TARGET_DIR" -maxdepth 1 -type d -name "cppcheck-*" 2>/dev/null | head -1)
if [ -z "$EXTRACTED_DIR" ]; then
  echo "✗ No cppcheck-* directory found after extraction"
  exit 1
fi

echo "Found extracted directory: $EXTRACTED_DIR"

ADDON_DIR="$EXTRACTED_DIR/addons"

# Check if addons directory exists
if [ ! -d "$ADDON_DIR" ]; then
  echo "✗ Addons directory does not exist: $ADDON_DIR"
  echo "Contents of extracted directory:"
  ls -la "$EXTRACTED_DIR" | head -20
  exit 1
fi

# Verify addons exist
if [ -f "$ADDON_DIR/misra.py" ]; then
  echo "✓ Found misra.py"
else
  echo "✗ misra.py not found"
  echo "Contents of addons directory:"
  ls -la "$ADDON_DIR" | head -20
  exit 1
fi

echo "✓ Addons ready at: $ADDON_DIR"
echo "$ADDON_DIR"
