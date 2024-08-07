UART_IDENT

FIUNKCJA PROGRAMU

Program wykrywa prędkość transmisji UART na podstawie odebranego bajtu i wykrywa, czy nadający moduł jest odsługiwany, po czym wysyła potwierdzenie (obsługiwany / nie obsługiwany) jeśli struktura bajtu powitalnego jest ok, w innym przypadku nie wysyła nic. 

WYNIKI

Załączone pliki /log wyniki pomiarów.png, waveforms.png, logs_conf.png, waveforms_conf.png/ pokazują odbiór kodu "0xA3" z potwierdzeniem oraz odbiór przypadkowego znaku (pokazany impuls pomiarowy okresu bitu) dla każdej prędkości w zakeresie 9600 - 115200 bps. W teście zmieniałem prędkość nadawania w terminalu TeraTerm lub Putty i otrzymywałem odpowiedź z kontrolera ARM w wyliczonej prędkości UART takiej samej jak predkość nadawania terminali.

RÓTKI OPIS JAK DZIAŁA PROGRAM 

W funkcji main znajduje się pętla while która może być przerwana dynamicznym timeoutem. Timeout jest dynamiczny ponieważ jego wartość jest uaktualniana wraz ze znalezieniem czasu trwania bitu /wartość min_period_value/. W zależności od prędkości transmisji bity mają różną szerokość. Na podstawie pomiaru szerokości bitu odbiornik jest w stanie obliczyć prędkość transmisji /funkcja rekrut_update_com_speed/. Pomiar szerokości bitu dokonywany jest poprzez odczyt timera T1 pomiędzy dwoma kolejnymi zboczami sygnału. Ponieważ znamy również nachylenie zboczy /np.: int_edge = IRQ_EDGE_IS_SET_FALLING;/, możemy określić czy sygnał pomiędzy nimi to jedynki czy zera. Wszystkie czytane wartości są zapisywane do tablicy /struct Bits_s measurements[]/. 
Po przekroczeniu timeoutu następuje obliczenie poszczególnych bitów na podstawie zapisanych czasów trwania mierzonych odcinków pomiędzy zboczami /funkcja rekrut_stop_measurement/ i obliczana jest wartość bajtu. Obliczony bajt porównywany jest z bajtami powitalnymi obsługiwanych modułów i jeżeli następuje zgodność tych bajtów to jest odpowiedź pozytywna 0xAA. W innym wypadku jeżeli struktura bajtów powitalnego jest prawidłowa ale moduł nie jest obsługiwany udzielana jest odpowiedź 0xAF. Odpowiedź wysyłana jest z aktualną, obliczoną prędkością.

Program działa realnie, brakuje natomiast różnego rodzaju detekcji i zabezpieczeń przed błędami lub nieoczekiwanymi zdarzeniami w transmisji.
