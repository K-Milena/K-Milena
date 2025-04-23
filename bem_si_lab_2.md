# Laboratorium 2 - Serwer WWW i architektura klucza publicznego

Pomogę Ci wykonać zadania krok po kroku. Oto komendy i instrukcje dla każdego zadania:

## Przygotowanie środowiska

1. Zaloguj się na konto `bemsi` (hasło podane na zajęciach)
2. Przejdź do katalogu roboczego:
   ```bash
   cd Q:/Users/bemsi
   ```
3. Sklonuj repozytorium (jeśli nie zostało to zrobione wcześniej):
   ```bash
   git clone https://gitlab-stud.elka.pw.edu.pl/103a-lbibm-isp-bemsi/bemsi-laboratorium-2.git lab2
   ```
4. Przejdź do katalogu lab2 i przygotuj środowisko:
   ```bash
   cd lab2
   ./prepare.sh
   ```

## Zadanie 1.1 - Utworzenie urzędu certyfikacji (CA)

### Wariant z easy-rsa (łatwiejszy):

1. Pobierz easy-rsa:
   ```bash
   git clone https://github.com/OpenVPN/easy-rsa.git
   cd easy-rsa/easyrsa3
   ```
2. Inicjalizuj CA:
   ```bash
   ./easyrsa init-pki
   ```
3. Zbuduj CA:
   ```bash
   ./easyrsa build-ca nopass
   ```
4. Wygeneruj listę CRL:
   ```bash
   ./easyrsa gen-crl
   ```

### Wariant z OpenSSL:

1. Utwórz klucz prywatny CA:
   ```bash
   openssl genpkey -algorithm RSA -out ca.key -aes256
   ```
2. Utwórz certyfikat CA:
   ```bash
   openssl req -x509 -new -nodes -key ca.key -sha256 -days 3650 -out ca.crt
   ```
3. Wygeneruj listę CRL:
   ```bash
   openssl ca -gencrl -keyfile ca.key -cert ca.crt -out crl.pem
   ```

## Zadanie 2.1 - Certyfikat dla serwera WWW

1. Wygeneruj klucz prywatny Ed25519:
   ```bash
   openssl genpkey -algorithm ED25519 -out server.key
   ```
2. Utwórz żądanie certyfikatu (CSR):
   ```bash
   openssl req -new -key server.key -out server.csr
   ```
   - W polu Common Name wpisz `myhost2.local`
3. Wygeneruj certyfikat podpisany przez CA:
   ```bash
   openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 365 -sha256 -extfile <(echo -e "subjectAltName=DNS:myhost2.local,IP:127.0.0.1")
   ```
4. Skopiuj certyfikaty do odpowiedniego katalogu:
   ```bash
   mkdir -p volumes/nginx/ssl
   cp server.crt server.key ca.crt volumes/nginx/ssl/
   ```

## Zadanie 2.2 - Weryfikacja certyfikatu

Sprawdź certyfikat:
```bash
openssl x509 -in server.crt -noout -text
```

## Zadanie 2.3 - Konfiguracja Nginx

1. Edytuj plik konfiguracyjny Nginx:
   ```bash
   nano volumes/nginx/nginx.conf
   ```
2. Dodaj następującą konfigurację SSL w zaznaczonym miejscu:
   ```nginx
   # BEGIN SSL CERTIFICATES
   ssl_certificate /etc/nginx/ssl/server.crt;
   ssl_certificate_key /etc/nginx/ssl/server.key;
   # END SSL CERTIFICATES
   ```
3. Uruchom serwer:
   ```bash
   ./start.sh
   ```
4. Sprawdź działanie:
   ```bash
   curl --cacert volumes/nginx/ssl/ca.crt --resolve myhost2.local:443:127.0.0.1 https://myhost2.local/
   curl --cacert volumes/nginx/ssl/ca.crt https://127.0.0.1/
   ```

## Zadanie 3.1 - Weryfikacja certyfikatu klienta

1. Zmodyfikuj plik konfiguracyjny Nginx, dodając:
   ```nginx
   ssl_client_certificate /etc/nginx/ssl/ca.crt;
   ssl_verify_client on;
   ssl_crl /etc/nginx/ssl/crl.pem;
   ```
2. Zrestartuj serwer:
   ```bash
   ./stop.sh
   ./start.sh
   ```
3. Sprawdź działanie (powinien zwrócić błąd 400):
   ```bash
   curl --cacert volumes/nginx/ssl/ca.crt --resolve myhost2.local:443:127.0.0.1 https://myhost2.local/
   ```

## Zadanie 4.1 - Certyfikat dla klienta

1. Wygeneruj klucz prywatny dla klienta:
   ```bash
   openssl genpkey -algorithm RSA -out klient1.key
   ```
2. Utwórz CSR:
   ```bash
   openssl req -new -key klient1.key -out klient1.csr
   ```
   - W polu Common Name wpisz `klient1`
3. Wygeneruj certyfikat:
   ```bash
   openssl x509 -req -in klient1.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out klient1.crt -days 365 -sha256
   ```

## Zadanie 4.2 - Weryfikacja certyfikatu klienta

Sprawdź certyfikat:
```bash
openssl x509 -in klient1.crt -noout -text
```

## Zadanie 4.3 - Test połączenia z certyfikatem klienta

```bash
curl --cacert volumes/nginx/ssl/ca.crt \
--resolve myhost2.local:443:127.0.0.1 \
--cert klient1.crt --key klient1.key \
https://myhost2.local/
```

## Zadanie 5.1 - Własny klient HTTPS (Python)

Oto przykładowy kod klienta w Pythonie:

```python
import requests

# Ścieżki do plików certyfikatów
ca_cert = 'volumes/nginx/ssl/ca.crt'
client_cert = 'klient1.crt'
client_key = 'klient1.key'

# Adres URL serwera
url = 'https://127.0.0.1'

# Wysłanie żądania GET
response = requests.get(
    url,
    verify=ca_cert,
    cert=(client_cert, client_key)
)

# Wyświetlenie odpowiedzi
print(response.text)
```

Zapisz kod jako `client.py` i uruchom:
```bash
python3 client.py
```

Pamiętaj, aby w sprawozdaniu umieścić wszystkie wymagane elementy, w tym zrzuty ekranu, zawartość plików konfiguracyjnych i wyniki działania poleceń.
