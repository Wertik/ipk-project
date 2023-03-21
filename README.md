# IPK Calculator Protocol Client (IPKCPC)

Klient pro komunikaci se serverem pomocí protokolu IPKC.

## Použití

`./ipkcpc [OPTIONS]`

Pro úspěšné připojení je nutné zadat minimálně `port`. Pro `host` je použita výchozí hodnota `localhost`, pro `mode` hodnota `TCP`.

__Options:__
```
--help|-hwastaken       Print this help page, exit.
--mode=VALUE|-m VALUE   Set the mode of communication. Supported modes: UDP, TCP.
--host=VALUE|-h VALUE   Set the host of the calculator server.
--port=VALUE|-p VALUE   Set the port of the calculator server.
```

## Struktura kódu

Soubory headerů mají koncovku `.hpp`, protože využívají funkcí a knihoven `C++`, nejsou interoperable s jazykem `C`.

Zpracování argumentů příkazové řádky je řešené vlastním jednoduchým parserem (`arg_parse.hpp/cpp`, `option.hpp/cpp`, `parse_result.hpp/cpp`, `match_result.hpp/cpp`). Implementace je generalizovaná pro případně využití i mimo implementaci IPKCP klienta. Parser podporuje pouze přepínače, ale ne poziční argumenty, ty nejsou v Projektu 1 potřeba.

Konkrétní využití pro potřeby klienta je separováno do souborů `cli.hpp/cpp`, kde se poskytnuté argumenty využijí pro sestavení konfigurace připojení k cílovému serveru `ServerConfig` (`server_config.hpp/cpp`, `server_mode.hpp/cpp`). Ta je poté využita pro vytvoření konkrétní implementace serveru podle specifikovaného módu (`UDP` => `UdpServer` / `TCP` => `TcpServer`).

Serverové třídy rozšiřují abstraktní třídu `Server`, která slouží jako rozhraní a definuje metody nutné pro vytvoření socketu a připojení, posílání zpráv, vyčkávání na odpověď a ukončení komunikace. Pro ukončení existují dvě možné funkce - `void end_gracefully()` & `void end()`. První z nich přes ukončením spojení provede nutnou komunikaci se serverem (to se děje pouze v případě TCP serveru), druhá provede okamžité uzavření socketu.

Pro zpracování signálu `C-c` (nebo `^C`, nebo `SIGINT`) je použita [sigaction](https://man7.org/linux/man-pages/man2/sigaction.2.html). Komunikace se ukončí před odesláním dalšího požadavku serveru*.

\* _V případě, že požadavek byl právě odeslán, klient čeká na odpověď serveru, až poté ukončí spojení._

## Navázání spojení

Vyhledání DNS záznamu, získání IP a vytvoření informace o cílové adrese a portu je stejné pro oba prokoly.

Pro vyhledání DNS záznamu je využitá funkce `gethostbyname`, podle linux man page je ale zastaralá a jsou preferovány jiné alternativy.

```
The gethostbyname*(), gethostbyaddr*(), herror(), and hstrerror() functions are obsolete. Applications should use getaddrinfo(3), getnameinfo(3), and gai_strerror(3) instead.
```
[man7.org](https://man7.org/linux/man-pages/man3/gethostbyname.3.html)

Socket je otevřen pomocí `int socket(domain, type, protocol)` ([man7.org](https://man7.org/linux/man-pages/man2/socket.2.html)) - v případě módu UDP s typem `SOCK_DGRAM`, v případě TCP s typem `SOCK_STREAM`.

Pro připojení socketu je použita funkce `int connect(sockfd, addr, addrlen)`, ta se chová jinak podle typu socketu specifikovaného parametrem `sockfd`. V případě `SOCK_STREAM` socketu otevře připojení podle specifikace protokolu a udržuje ho. Pokud je socket typu `SOCK_DGRAM`, argumenty `addr` a `addrlen` jsou použity jako adresa až při volání `send` & `recv`, není tedy nutné je specifikovat při každém volání těchto funkcí.

```
If the socket sockfd is of type SOCK_DGRAM, then addr is the address to which datagrams are sent by default, and the only address from which datagrams are received. [...]
```
[man7.org](https://man7.org/linux/man-pages/man2/connect.2.html)

## Funkčnost nad rámec zadání

Implementace je rozšířena pouze o QoL maličkosti -- přidána help page s použitím programu a možnost použít i dlouhé verze přepínačů.
(Např.: `./ipkcpc --mode=UDP --host=localhost --port=2065`)
