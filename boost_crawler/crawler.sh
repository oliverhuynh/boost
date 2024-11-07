if [[ -f /tmp/doit ]]; then
  if [[ ! -f /tmp/doingit ]]; then
    touch /tmp/doingit
    rm -rf /tmp/crawler;
    wget --mirror -p --convert-links -P /tmp/crawler -R css,js,pdf 'https://kobelco-compressors.com/'
    rm /tmp/doit
    rm /tmp/doingit
  fi
fi