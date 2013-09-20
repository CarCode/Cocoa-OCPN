for file in *.jpg ; do convert "$file" "${file/%jpg/png}" ; done
