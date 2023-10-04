Naravno! Da bismo dodali funkcionalnost za prikaz unetog Morse koda na LCD-u, moramo dodati funkcionalnost za upravljanje LCD-om. Pretpostavljam da koristite neki generički LCD koji se može upravljati preko I2C ili GPIO.

U ovom primeru, koristićemo izmišljeni API za LCD (zamislite da imamo funkcije lcd_init(), lcd_clear(), i lcd_print()) da bismo demonstrirali kako bi to izgledalo:

c
Copy code
#include <string.h>
#include <wiringPi.h>
#include <unistd.h>

#define BUTTON_DOT_PIN 2  
#define BUTTON_DASH_PIN 3

char morseBuffer[7];  

void lcd_init() {
    // Ovde bi inicijalizovali LCD. Ovo je izmišljena funkcija.
}

void lcd_clear() {
    // Ova funkcija bi čistila LCD ekran. Ovo je izmišljena funkcija.
}

void lcd_print(const char *message) {
    // Ova funkcija bi štampala poruku na LCD ekranu. Ovo je izmišljena funkcija.
}

char morseToChar(const char* morse) {
    for(int j = 0; j < sizeof(morseAlphabet) / sizeof(MorseCode); j++) {
        if(strcmp(morse, morseAlphabet[j].morse) == 0) {
            return morseAlphabet[j].letter;
        }
    }
    return '?';  // nepoznat Morse kod
}

void inputMorse() {
    lcd_init();
    int idx = 0;
    memset(morseBuffer, 0, sizeof(morseBuffer));
    
    while(1) {
        if(digitalRead(BUTTON_DOT_PIN) == HIGH) {
            morseBuffer[idx++] = '.';
            lcd_clear();
            lcd_print(morseBuffer);
            usleep(300000);
        } else if(digitalRead(BUTTON_DASH_PIN) == HIGH) {
            morseBuffer[idx++] = '-';
            lcd_clear();
            lcd_print(morseBuffer);
            usleep(300000);
        }

        if(idx == 6) {
            char decodedChar = morseToChar(morseBuffer);
            char msg[2] = {decodedChar, '\0'};
            lcd_clear();
            lcd_print(msg);
            idx = 0;
            memset(morseBuffer, 0, sizeof(morseBuffer));
            usleep(1000000);  // pauza od 1 sekundu da bi korisnik video rezultat
        }
    }
}
Važno je napomenuti da ovaj kod zavisi od morseAlphabet strukture koja je definisana u prethodnom odgovoru.

Ako koristite stvarnu biblioteku za upravljanje LCD-om (kao što je lcd.h za WiringPi), trebalo bi da zamenite izmišljene funkcije (lcd_init, lcd_clear, lcd_print) sa odgovarajućim funkcijama iz te biblioteke.





Naravno! S obzirom da ste naveli Raspberry Pi i DVK512, pretpostavljam da koristite WiringPi biblioteku za upravljanje GPIO pinovima. Ako koristite neku drugu biblioteku, kod će biti sličan, ali će se pozivi funkcija možda razlikovati.

Prvo, kreirajmo Morzeovu azbuku:

c
Copy code
#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <unistd.h>

#define LED_PIN 1  // Asumiram da je LED povezana na GPIO 1 (WiringPi oznaka)
#define BUTTON_DOT_PIN 2  // Asumiram da je taster za '.' na GPIO 2
#define BUTTON_DASH_PIN 3  // Asumiram da je taster za '-' na GPIO 3

typedef struct {
    char letter;
    const char* morse;
} MorseCode;

MorseCode morseAlphabet[] = {
    {'A', ".-"},
    {'B', "-..."},
    {'C', "-.-."},
    {'D', "-.."},
    {'E', "."},
    {'F', "..-."},
    {'G', "--."},
    {'H', "...."},
    {'I', ".."},
    {'J', ".---"},
    {'K', "-.-"},
    {'L', ".-.."},
    {'M', "--"},
    {'N', "-."},
    {'O', "---"},
    {'P', ".--."},
    {'Q', "--.-"},
    {'R', ".-."},
    {'S', "..."},
    {'T', "-"},
    {'U', "..-"},
    {'V', "...-"},
    {'W', ".--"},
    {'X', "-..-"},
    {'Y', "-.--"},
    {'Z', "--.."},
    {'0', "-----"},
    {'1', ".----"},
    {'2', "..---"},
    {'3', "...--"},
    {'4', "....-"},
    {'5', "....."},
    {'6', "-...."},
    {'7', "--..."},
    {'8', "---.."},
    {'9', "----."}
};
Za LED diodu:

c
Copy code
void sendMorse(const char* code) {
    for(int i = 0; code[i]; i++) {
        if(code[i] == '.') {
            digitalWrite(LED_PIN, HIGH);
            usleep(100000);  // 100ms za tacku
            digitalWrite(LED_PIN, LOW);
        } else if(code[i] == '-') {
            digitalWrite(LED_PIN, HIGH);
            usleep(300000);  // 300ms za crtu
            digitalWrite(LED_PIN, LOW);
        }
        usleep(100000);  // pauza između tačke/crte
    }
    usleep(300000);  // pauza između slova
}

void encodeText(const char* text) {
    for(int i = 0; text[i]; i++) {
        for(int j = 0; j < sizeof(morseAlphabet) / sizeof(MorseCode); j++) {
            if(toupper(text[i]) == morseAlphabet[j].letter) {
                sendMorse(morseAlphabet[j].morse);
                break;
            }
        }
    }
}
Za unos preko tastera:

c
Copy code
char morseBuffer[7];  // najduži Morse kod je za '0' sa 5 crta

void inputMorse() {
    int idx = 0;
    memset(morseBuffer, 0, sizeof(morseBuffer));
    
    while(1) {  // beskonačna petlja dok ne unesete celu poruku
        if(digitalRead(BUTTON_DOT_PIN) == HIGH) {
            morseBuffer[idx++] = '.';
            // prikaz na LCD ... (implementacija zavisi od vašeg LCD-a)
            usleep(300000);
        } else if(digitalRead(BUTTON_DASH_PIN) == HIGH) {
            morseBuffer[idx++] = '-';
            // prikaz na LCD ... 
            usleep(300000);
        }

        if(idx == 6) {
            // proverite morseBuffer i prikažite odgovarajuće slovo na LCD-u
            // možete koristiti sličan pristup kao kod LED diode
            // ...
            idx = 0;
            memset(morseBuffer, 0, sizeof(morseBuffer));
        }
    }
}
Konačno, u main funkciji:

c
Copy code
int main() {
    wiringPiSetup();
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_DOT_PIN, INPUT);
    pinMode(BUTTON_DASH_PIN, INPUT);

    char choice;
    printf("Choose mode (e for encode, d for decode): ");
    scanf(" %c", &choice);

    if(choice == 'e') {
        char text[100];
        printf("Enter text: ");
        scanf(" %99[^\n]", text);
        encodeText(text);
    } else if(choice == 'd') {
        inputMorse();
    }
    
    return 0;
}
Važno: Ovo je osnovni okvir koji pokazuje kako biste mogli da postupite. Morate da prilagodite kod prema stvarnom hardverskom okruženju, dodate odgovarajuće biblioteke za LCD displej i eventualno dodate debounce logiku za tastere.
