# Rozwiązanie ćwiczenia BEMSI Laboratorium 3 - Zapora sieciowa w systemie Linux

Poniżej przedstawiam komendy krok po kroku do wykonania poszczególnych zadań z ćwiczenia:

## Przygotowanie środowiska

1. Pobranie repozytorium i budowa środowiska:
```bash
git clone https://gitlab-stud.elka.pw.edu.pl/103a-lbibm-isp-bemsi/bemsi-laboratorium-3.git lab3
cd lab3
./build.sh
```

2. Uruchomienie kontenerów:
```bash
./start.sh
```

3. Wejście do kontenera iproxy:
```bash
./run-bash.sh iproxy
```

## Zadania Z1.X - Konfiguracja części lokalnej zapory

### Z1.1 - SSH dostępny tylko dla iclient1 i iclient2
```bash
iptables -A INPUT -p tcp --dport 22 -s 10.22.20.3 -j ACCEPT
iptables -A INPUT -p tcp --dport 22 -s 10.22.20.4 -j ACCEPT
iptables -A INPUT -p tcp --dport 22 -j DROP
```

### Z1.2 - WWW na porcie 80 dostępna dla wszystkich
```bash
iptables -A INPUT -p tcp --dport 80 -j ACCEPT
```

### Z1.3 - WWW na porcie 8080 dostępna tylko dla iclient1
```bash
iptables -A INPUT -p tcp --dport 8080 -s 10.22.20.3 -j ACCEPT
iptables -A INPUT -p tcp --dport 8080 -j REJECT --reject-with icmp-port-unreachable
```

### Z1.4 - WWW na losowym porcie dostępna tylko lokalnie
```bash
# Najpierw znajdź numer portu:
netstat -lnp | grep bemsi-web-app
# Załóżmy, że to port 12345
iptables -A INPUT -p tcp --dport 12345 -s 127.0.0.1 -j ACCEPT
iptables -A INPUT -p tcp --dport 12345 -j DROP
```

### Z1.5 - Wszystkie usługi dostępne lokalnie
```bash
iptables -A INPUT -i lo -j ACCEPT
```

### Z1.6 - ICMP Echo request i reply akceptowane, inne odrzucane i logowane
```bash
iptables -A INPUT -p icmp --icmp-type echo-request -j ACCEPT
iptables -A INPUT -p icmp --icmp-type echo-reply -j ACCEPT
iptables -A INPUT -p icmp -j LOG --log-prefix "ICMP DROPPED: "
iptables -A INPUT -p icmp -j DROP
```

### Z1.7 - Weryfikacja dostępu do internetu
```bash
curl 194.29.160.106
# Jeśli nie działa, dodaj:
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
```

## Zadania Z2.X - Konfiguracja DNAT

### Dodanie wymaganej reguły SNAT
```bash
iptables -t nat -A POSTROUTING -o eth1 -j SNAT --to-source 10.22.88.1
```

### Z2.1 - SSH na porcie 2222 tylko dla iclient1 i iclient2
```bash
iptables -A FORWARD -p tcp --dport 22 -d 10.22.88.2 -s 10.22.20.3 -j ACCEPT
iptables -A FORWARD -p tcp --dport 22 -d 10.22.88.2 -s 10.22.20.4 -j ACCEPT
iptables -A FORWARD -p tcp --dport 22 -d 10.22.88.2 -j DROP
iptables -t nat -A PREROUTING -p tcp --dport 2222 -j DNAT --to-destination 10.22.88.2:22
```

### Z2.2 - WWW na porcie 8088 dostępna dla wszystkich
```bash
iptables -A FORWARD -p tcp --dport 80 -d 10.22.88.2 -j ACCEPT
iptables -t nat -A PREROUTING -p tcp --dport 8088 -j DNAT --to-destination 10.22.88.2:80
```

## Zadanie Z3.1 - Konfiguracja SNAT dla itarget
```bash
iptables -t nat -A POSTROUTING -o eth0 -j SNAT --to-source 10.22.20.2
```

## Zapisywanie reguł
Po skonfigurowaniu wszystkich reguł należy je zapisać:
```bash
save-rules
```

## Weryfikacja
Do weryfikacji można użyć poleceń:
```bash
# Sprawdzenie reguł iptables
iptables -L -n -v
iptables -t nat -L -n -v

# Testowanie połączeń z innych kontenerów
./run-bash.sh iclient1
curl 10.22.20.2:80      # Powinno działać
curl 10.22.20.2:8080    # Powinno działać tylko z iclient1
curl 10.22.20.2:2222    # Powinno przekierować do itarget SSH
```

## Zatrzymanie środowiska
Po zakończeniu pracy:
```bash
./stop.sh
```

Pamiętaj, aby wszystkie wykonane polecenia i ich wyniki zapisać w sprawozdaniu wraz z zawartością pliku `volumes/proxy/iptables-rules`.
