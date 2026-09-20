

##Setup


zuerst versucht man, das ganze Node.js Zeugs ans laufen zu kriegen und folgt den Anleitungen für Revovery und Installation auf http://koehlers.de/wiki/doku.php?id=pc:vueprojects


## Freund Einladungen als Shareable URLs

(Testpage für Sharing: https://w3c.github.io/web-share/demos/share-files.html)

Folgender Plan:

Auf dem Server wird von der User-uudid ein fingerprint gemacht (FP) 32 Bit = 4 Bytes = 8 Chars (crc32(serialize($arr)) https://stackoverflow.com/a/53306323/1927807)

Einem (Pseudo-) Random- Wert aus dem Timestamp 32 Bit = 4 Bytes = 8 Chars (TS)

Der sha256 Hash  (SHA) aus dem Fingerprint,  dem Timestamp und dem Salt: 256 Bits = 32 Bytes = 64 chars

dann noch der URL zum Abruf "https://frienzz.net/#/userpair?d=" = 33 Bytes

Macht zusammen:

33 Bytes (URL) + url_coded_ base64 (4 Bytes (FP) + 2 Bytes (AM) + 4 Bytes (RD) + 32 Bytes (SHA) ) = 56 = total 91 Bytes 

Also 91 Bytes


Wenn dieser Gutschein dann zum Pairing verwendet wird, wird seine "Identität" (Die Kombination aus Monat (AM) und Random (RD) ) in die User Pairing Tabelle mit gespeichert und so als nicht mehr benutzbar 


Beispiel-Javascript-Code zum Erzeugen eines Shares mit Image: https://stackoverflow.com/a/68503838/1927807 oder auch https://developer.mozilla.org/en-US/docs/Web/Progressive_web_apps/How_to/Share_data_between_apps

## CAD: Bleche winkeln
https://www.youtube.com/watch?v=Sj2R9wKrOIM



Artikel über Serviceworker Push- Dienste: https://freiheit.f4.htw-berlin.de/ikt/pushnotes/#erzeugen-einer-neuen-subscription