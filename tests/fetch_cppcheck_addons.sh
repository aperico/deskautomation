#!/bin/bash
# Download and extract cppcheck addons (misra.py, cert.py)
# Usage: ./fetch_cppcheck_addons.sh [target_dir]

set -e  # Exit on any error

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
  
  # Download with fail-on-error and show progress
  HTTP_CODE=$(curl -sL -w "%{http_code}" -f --max-time 60 "$URL" -o "$TARGET_DIR/cppcheck.zip" 2>&1 | tail -1 || echo "000")
  
  if [ "$HTTP_CODE" = "200" ] && [ -f "$TARGET_DIR/cppcheck.zip" ]; then
    # Check file size (should be > 1MB)
    FILE_SIZE=$(stat -f%z "$TARGET_DIR/cppcheck.zip" 2>/dev/null || stat -c%s "$TARGET_DIR/cppcheck.zip" 2>/dev/null || echo "0")
    
    if [ "$FILE_SIZE" -gt 1000000 ]; then
      # Verify zip integrity
      if unzip -t "$TARGET_DIR/cppcheck.zip" > /dev/null 2>&1; then
        echo "✓ Download successful (${FILE_SIZE} bytes, HTTP $HTTP_CODE)"
        SUCCESS=1
        break
      else
        echo "✗ Zip file corrupted (integrity check failed)"
      fi
    else
      echo "✗ File too small ($FILE_SIZE bytes), likely error page"
      cat "$TARGET_DIR/cppcheck.zip" | head -5 || true
    fi
  else
    echo "✗ Download failed (HTTP $HTTP_CODE)"
  fi
  
  rm -f "$TARGET_DIR/cppcheck.zip"
done

if [ $SUCCESS -eq 0 ]; then
  echo "✗ Failed to download cppcheck addons from all sources"
  echo "Attempted URLs:"
  printf '%s\n' "${URLS[@]}"
  exit 1
fi

# Clean old extraction
rm -rf "$TARGET_DIR"/cppcheck-*

# Extract
echo "Extracting addons..."
unzip -q "$TARGET_DIR/cppcheck.zip" -d "$TARGET_DIR"

# Find extracted directory
EXTRACTED_DIR=$(find "$TARGET_DIR" -maxdepth 1 -type d -name "cppcheck-*" | head -1)
if [ -z "$EXTRACTED_DIR" ]; then
  echo "✗ Failed to find extracted cppcheck directory"
  echo "Contents of $TARGET_DIR:"
  ls -la "$TARGET_DIR"
  exit 1
fi

ADDON_DIR="$EXTRACTED_DIR/addons"
echo "Found addons directory: $ADDON_DIR"

# Verify addons exist
if [ -f "$ADDON_DIR/misra.py" ]; then
  echo "✓ Found misra.py"
else
  echo "✗ misra.py not found at $ADDON_DIR/misra.py"
  echo "Contents of $ADDON_DIR:"
  ls -la "$ADDON_DIR" || echo "Directory does not exist"
  exit 1
fi

if [ -f "$ADDON_DIR/cert.py" ]; then
  echo "✓ Found cert.py"
else
  echo "⚠ cert.py not found (continuing without CERT checks)"
fi

echo "✓ Addons ready at: $ADDON_DIR"
echo "$ADDON_DIR"
