#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include<windows.h>

extern "C" {
#include"SDL2-2.0.10\include\SDL.h"
#include"SDL2-2.0.10\include\SDL_main.h"
}

#define SCREEN_WIDTH	532
#define SCREEN_HEIGHT	612
#define GRUBOSC_DOLNA   65+ 92
#define GRUBOSC_GORNA   115 + 92
#define PRZERWA         35
#define GORA            0
#define PRAWA           1
#define LEWA            2
#define DOL             3
#define JEDNO_POLE      38
#define START_Y			500
#define CZAS			50
#define DLUGOSC_BOXA	300




// narysowanie napisu txt na powierzchni screen, zaczynaj¹c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj¹ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt œrodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32 *)p = color;
	};


// rysowanie linii o d³ugoœci l w pionie (gdy dx = 0, dy = 1) 
// b¹dŸ poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// rysowanie prostok¹ta o d³ugoœci boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

typedef struct {
	int x;
	int y;
	int polozenie_x;
	int polozenie_y;
	bool narysowany = false;
	int ilosc_obiektow;
	int predkosc;
}obiekt;

void Nadaj_wartosci(obiekt *obiekt, int x, int y, int polozenie_x_pocz, int polozenie_y_pocz, int ile_obiektow, int predkosc)
{
	for (int i = 0; i < ile_obiektow; i++)
	{
		obiekt[i].x = x;
		obiekt[i].y = y;
		obiekt[i].polozenie_x = polozenie_x_pocz;
		obiekt[i].polozenie_y = polozenie_y_pocz;
		obiekt[i].ilosc_obiektow = ile_obiektow;
		obiekt[i].predkosc = predkosc;
	}
}
void czy_poza_ekranem(obiekt* obiekt, int z_ktorej_strony,int ile_obiektow)
{
	for (int i = 0; i < ile_obiektow; i++)
	{
		if (z_ktorej_strony == PRAWA)if (obiekt[i].polozenie_x <= 0 - (SCREEN_WIDTH + obiekt[i].x/2))obiekt[i].polozenie_x = (SCREEN_WIDTH + obiekt[i].x/2);
		if (z_ktorej_strony == LEWA)if (obiekt[i].polozenie_x >= (2 * SCREEN_WIDTH) + obiekt[i].x/2)obiekt[i].polozenie_x = 0 - obiekt[i].x/2;
	}
}
void narysuj_rzad(obiekt* obiekt, int ile_obiektow, SDL_Surface* screen, SDL_Surface* rysunek, int roznica, int strona)
{
	///rysuje pierwszy obiekt z jednego rzedu
	DrawSurface(screen, rysunek, obiekt[0].polozenie_x += obiekt[0].predkosc, obiekt[0].polozenie_y);
	obiekt[0].narysowany = true;
	for (int i = 1; i < ile_obiektow/2 ; i++)
	{
		///zaleznie od strony od ktorej obiekt zaczyna sie po przebyciu 'roznicy' rysuje nastepny jezeli jeszcze nie byl narysowany
		if (((obiekt[i - 1].polozenie_x >= roznica && strona == LEWA) || (strona == PRAWA && obiekt[i - 1].polozenie_x <= SCREEN_WIDTH - roznica)) && obiekt[i].narysowany == false)
		{
			obiekt[i].narysowany = true;
			DrawSurface(screen, rysunek, obiekt[i].polozenie_x += obiekt[i].predkosc, obiekt[i].polozenie_y);
		}
		else if (obiekt[i].narysowany == true)
		{
			DrawSurface(screen, rysunek, obiekt[i].polozenie_x += obiekt[i].predkosc, obiekt[i].polozenie_y);
		}
	}///rzad jest podzielony na dwie czesci obiektow(pierwsza zajmuje ekran najpierw a druga zajmuje ekran jak pierwsza przejezdza swoja druga wirtualna droge)
	if(strona == PRAWA)if (obiekt[0].polozenie_x <= 0)obiekt[ile_obiektow / 2].narysowany = true;
	if(strona == LEWA)if (obiekt[0].polozenie_x >= SCREEN_WIDTH )obiekt[ile_obiektow / 2].narysowany = true;
	if (obiekt[ile_obiektow / 2].narysowany == true)DrawSurface(screen, rysunek, obiekt[ile_obiektow / 2].polozenie_x += obiekt[ile_obiektow / 2].predkosc, obiekt[ile_obiektow / 2].polozenie_y);
	for (int i = (ile_obiektow / 2)+1; i < ile_obiektow ; i++)
	{//tutaj to co u gory powtorzine tylko dla drugiej czesci
		if ((obiekt[i - 1].polozenie_x >= roznica && strona == LEWA) ||( strona == PRAWA && obiekt[i - 1].polozenie_x <= SCREEN_WIDTH - roznica) && obiekt[i].narysowany == false)
		{
			obiekt[i].narysowany = true;
			DrawSurface(screen, rysunek, obiekt[i].polozenie_x += obiekt[i].predkosc, obiekt[i].polozenie_y);
		}
		else if (obiekt[i].narysowany == true)
		{
			DrawSurface(screen, rysunek, obiekt[i].polozenie_x += obiekt[i].predkosc, obiekt[i].polozenie_y);
		}
	}
}
int sprawdz_kolizje(obiekt zaba, obiekt* obiekt,int ile_obiektow)
{

	for (int i = 0; i < ile_obiektow; i++)
	{
		///najpierw sprawdza czy zaba jest w rzedzie obiektu a pozniej sprawdza x zaby i samochodzika
		if ((zaba.polozenie_y+(zaba.y/2) <= obiekt[i].polozenie_y+obiekt[i].y && zaba.polozenie_y+(zaba.y / 2) >= (obiekt[i].polozenie_y - obiekt[i].y/2) ) || ((zaba.polozenie_y -zaba.y/2) <= obiekt[i].polozenie_y+obiekt[i].y/2 && (zaba.polozenie_y - zaba.y/2) >= (obiekt[i].polozenie_y - obiekt[i].y/2)))
		{//sprawdzenie dwoch przypadkow czyli zaby od ogry kiedy dolna granica zabki jest miedzy autkiem a drugi to kiedy grona granica jest miedzy autkiem
			if (zaba.polozenie_x+zaba.x/2 >= (obiekt[i].polozenie_x - obiekt[i].x/2) && zaba.polozenie_x+zaba.x/2 <= obiekt[i].polozenie_x+ obiekt[i].x/2) return 1;
			else if ((zaba.polozenie_x - zaba.x/2) <= obiekt[i].polozenie_x+obiekt[i].x/2 && (zaba.polozenie_x - zaba.x/2) >= (obiekt[i].polozenie_x - obiekt[i].x/2))return 1;
		}
	}
	return 0;
}
int czy_wapdla_do_wody(obiekt& zaba, obiekt* obiekt, int ile_obiektow,int z_ktorej_strony)
{
	//sprawdzenie y zaby z y obiektu
	if (zaba.polozenie_y <= obiekt[0].polozenie_y + obiekt[0].y / 2 && zaba.polozenie_y  >= obiekt[0].polozenie_y - obiekt[0].y / 2)
	{
		for (int i = 0; i < ile_obiektow; i++)
		{
			if (zaba.polozenie_y <= obiekt[i].polozenie_y + obiekt[i].y / 2 && zaba.polozenie_y  >= obiekt[i].polozenie_y - obiekt[i].y / 2)
			{//sprawdzenie czy znajduje sie na jakims obiekcie
				if (zaba.polozenie_x>= obiekt[i].polozenie_x - obiekt[i].x/2 && zaba.polozenie_x <= obiekt[i].polozenie_x + obiekt[i].x/2)
				{
					zaba.polozenie_x+= obiekt[i].predkosc;
					return 0;
				}
			}
		}
		return 1;
	}
}
int meta(obiekt zaba, obiekt* punkt, SDL_Surface* screen, SDL_Surface* rysunek)
{
	bool sprawdz = false;
	//jesli wszystkie sa juz zdobyte zwroc 3
	for (int i = 0; i < punkt[0].ilosc_obiektow; i++)
	{
		if(punkt[i].narysowany == false)break;
		if (i == 4)return 3;
	}
		for (int i = 0; i < punkt[0].ilosc_obiektow; i++)
		{
			//sprawdzenie y zaby z y punktu
			if (punkt[i].narysowany == true)DrawSurface(screen, rysunek, punkt[i].polozenie_x, punkt[i].polozenie_y);
			if (zaba.polozenie_y <= punkt[0].polozenie_y + punkt[0].y && zaba.polozenie_y >= punkt[0].polozenie_y - punkt[0].y)
			{
				if (zaba.polozenie_x <= punkt[i].polozenie_x + punkt[i].x && zaba.polozenie_x >= punkt[i].polozenie_x - punkt[i].x)
				{	//sprawdza czy zaba jest na x jednego z punktow
					sprawdz = true;
					if (punkt[i].narysowany == false)
					{
						punkt[i].narysowany = true;
						return 0;
					}
					else if (punkt[i].narysowany == true)
					{
						return 1;
					}
				}
			}
		}//jezeli nie byl na zadnym dobrym x punktu a znajduje sie na y to zwraca 1 czyli kolizje
		if ((zaba.polozenie_y <= punkt[0].polozenie_y + punkt[0].y && zaba.polozenie_y >= punkt[0].polozenie_y - punkt[0].y) && sprawdz == false) return 1;
	return 2;
}
int puste_zdj(SDL_Surface*zdjecie, SDL_Surface* screen, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer)
{
	if (zdjecie == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	return 0;
}
// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	int t1, t2, quit, frames, rc;
	double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
	SDL_Event event;
	SDL_Surface* screen, * charset, * bitmapa, *pkt, *zycie;
	SDL_Surface* frogger, *frogger_prawo, *frogger_lewo, *frogger_dol;
	SDL_Surface *auto_1, *auto_2, *auto_3, *auto_4, *auto_5;
	SDL_Surface* woda_1, *woda_2,*woda_3, *woda_4, *woda_5;
	SDL_Surface *eti, *bombastic, *notbad,*nochal;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;


	// okno konsoli nie jest widoczne, je¿eli chcemy zobaczyæ
	// komunikaty wypisywane printf-em trzeba w opcjach:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// zmieniæ na "Console"
	// console window is not visible, to see the printf output
	// the option:
	// project -> szablon2 properties -> Linker -> System -> Subsystem
	// must be changed to "Console"
	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	// tryb pe³noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "Szablon do zdania drugiego 2017");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
	                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                           SDL_TEXTUREACCESS_STREAMING,
	                           SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);


	// wczytanie obrazka cs8x8.bmp
	/*charset = SDL_LoadBMP("cs8x8.bmp");
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(charset, true, 0x000000);

	eti = SDL_LoadBMP("eti.bmp");
	if(eti == NULL) {
		printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_DestroyTexture(scrtex);
		SDL_DestroyWindow(window);
		SDL_DestroyRenderer(renderer);
		SDL_Quit();
		return 1;
		};*/

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	//distance = 0;
	//etiSpeed = 1;

	bitmapa = SDL_LoadBMP("mapa.bmp");
	auto_1 = SDL_LoadBMP("auto1.bmp");
	auto_2 = SDL_LoadBMP("auto2.bmp");
	auto_3 = SDL_LoadBMP("auto3.bmp");
	auto_4 = SDL_LoadBMP("auto4.bmp");
	auto_5 = SDL_LoadBMP("auto5.bmp");
	woda_1 = SDL_LoadBMP("woda1.bmp");
	woda_2 = SDL_LoadBMP("woda2.bmp");
	woda_3 = SDL_LoadBMP("woda3.bmp");
	woda_4 = SDL_LoadBMP("woda4.bmp");
	woda_5 = SDL_LoadBMP("woda5.bmp");
	frogger = SDL_LoadBMP("2head.bmp");
	frogger_prawo = SDL_LoadBMP("2head.bmp");
	frogger_lewo = SDL_LoadBMP("2head.bmp");
	frogger_dol = SDL_LoadBMP("2head.bmp");
	pkt = SDL_LoadBMP("piesel.bmp");
	zycie = SDL_LoadBMP("spiacy.bmp");
	charset = SDL_LoadBMP("cs8x8.bmp");
	bombastic = SDL_LoadBMP("Bombastic.bmp");
	notbad = SDL_LoadBMP("notbad.bmp");
	nochal = SDL_LoadBMP("nochal.bmp");
	if (puste_zdj(bitmapa, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(auto_1, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(auto_2, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(auto_3, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(auto_4, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(auto_5, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(woda_1, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(woda_2, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(woda_3, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(woda_4, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(woda_5, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(frogger, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(frogger_prawo, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(frogger_lewo, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(frogger_dol, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(pkt, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(zycie, screen, scrtex, window, renderer) == 1)return 1;
	if (puste_zdj(charset, screen, scrtex, window, renderer) == 1)return 1;
	SDL_SetColorKey(charset, true, 0x000000);
	
	obiekt punkt[5];
	Nadaj_wartosci(punkt, 30, 30, 40, 45, 5 ,0);
	for (int i = 0; i < punkt[0].ilosc_obiektow; i++)
	{
		punkt[i].polozenie_x += (112.5 * i);

	}

	obiekt auto1[6];
	Nadaj_wartosci(auto1, 26, 30, SCREEN_WIDTH + 13, SCREEN_HEIGHT - (GRUBOSC_DOLNA), 6 ,-1);
	obiekt auto2[4];
	Nadaj_wartosci(auto2, 33, 30, 0 - 33/2, SCREEN_HEIGHT - (GRUBOSC_DOLNA + PRZERWA), 4 ,1);
	obiekt auto3[6];
	Nadaj_wartosci(auto3, 42, 30, SCREEN_WIDTH + 42/2, SCREEN_HEIGHT - (GRUBOSC_DOLNA + 2 * PRZERWA), 6, -1);
	obiekt auto4[2];
	Nadaj_wartosci(auto4, 34, 30, 0 - 34/2, SCREEN_HEIGHT - (GRUBOSC_DOLNA + 3 * PRZERWA), 2 ,3);
	obiekt auto5[4];
	Nadaj_wartosci(auto5, 71, 30, SCREEN_WIDTH + 71/2, SCREEN_HEIGHT - (GRUBOSC_DOLNA + 4 * PRZERWA), 4 ,-1);


	obiekt woda1[6];
	Nadaj_wartosci(woda1, 129, 30, SCREEN_WIDTH + 129/2, SCREEN_HEIGHT - (GRUBOSC_GORNA + 5 * PRZERWA), 6 ,-1);
	obiekt woda2[4];
	Nadaj_wartosci(woda2, 123, 30, 0 - 123/2, SCREEN_HEIGHT - (GRUBOSC_GORNA + 6 * PRZERWA), 4 ,1);
	obiekt woda3[2];
	Nadaj_wartosci(woda3, 264, 30, 0 - 264/2, SCREEN_HEIGHT - (GRUBOSC_GORNA + 7 * PRZERWA),2 ,2);
	obiekt woda4[6];
	Nadaj_wartosci(woda4, 85, 30, SCREEN_WIDTH + 85/2, SCREEN_HEIGHT - (GRUBOSC_GORNA + 8 * PRZERWA), 6 ,-1);
	obiekt woda5[4];
	Nadaj_wartosci(woda5, 177, 30, 0 - 177/2, SCREEN_HEIGHT - (GRUBOSC_GORNA + 9 * PRZERWA), 4 , 1);

	obiekt zaba;
		zaba.x = 20;
		zaba.y = 20;
		zaba.polozenie_x = SCREEN_WIDTH / 2;
		zaba.polozenie_y = START_Y;
		zaba.predkosc = 38;

		int kierunek = GORA;
		int zycia = 5;
		bool pauza = false;
		bool wyjscie = false;
		int punktacja = 0;
		bool czy_byl[11];
		for (int i = 0; i < 11; i++)czy_byl[i] = false;
		double czas_wyswietlania = 0;
		int bonus = 0;

	while(!quit) {
		
		SDL_FillRect(screen, NULL, czarny);
		int kolizja = 0;
		if (czas_wyswietlania < 0)czas_wyswietlania = 0;
		
		czy_poza_ekranem(auto1, PRAWA, auto1[0].ilosc_obiektow);
		czy_poza_ekranem(auto2, LEWA, auto2[0].ilosc_obiektow);
		czy_poza_ekranem(auto3, PRAWA, auto3[0].ilosc_obiektow);
		czy_poza_ekranem(auto4, LEWA, auto4[0].ilosc_obiektow);
		czy_poza_ekranem(auto5, PRAWA, auto5[0].ilosc_obiektow);
		
		czy_poza_ekranem(woda1, PRAWA, woda1[0].ilosc_obiektow);
		czy_poza_ekranem(woda2, LEWA, woda2[0].ilosc_obiektow);
		czy_poza_ekranem(woda3, LEWA, woda3[0].ilosc_obiektow);
		czy_poza_ekranem(woda4, PRAWA, woda4[0].ilosc_obiektow);
		czy_poza_ekranem(woda5, LEWA, woda5[0].ilosc_obiektow);

		DrawSurface(screen, bitmapa, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

		///rysowanie co 28 pikseli zycia
		int przerwa = 0;
		if (czas_wyswietlania > 0)
		{
			for (int i = 0; i < zycia; i++)
			{
				DrawSurface(screen, nochal, przerwa + punkt[0].x / 2, (SCREEN_HEIGHT - 60));
				przerwa += 28;
			}
		}
		else
		{
			for (int i = 0; i < zycia; i++)
			{
				DrawSurface(screen, zycie, przerwa + punkt[0].x / 2, (SCREEN_HEIGHT - 60));
				przerwa += 28;
			}
		}

		narysuj_rzad(auto1, auto1[0].ilosc_obiektow, screen, auto_1, 150, PRAWA);
		narysuj_rzad(auto2, auto2[0].ilosc_obiektow, screen, auto_2, 350, LEWA);
		narysuj_rzad(auto3, auto3[0].ilosc_obiektow, screen, auto_3, 120, PRAWA);
		narysuj_rzad(auto4, auto4[0].ilosc_obiektow, screen, auto_4, 300, LEWA);
		narysuj_rzad(auto5, auto5[0].ilosc_obiektow, screen, auto_5, 250, PRAWA);
		
		if(zycia == 0)///jesli koniec gry to narysuj boxa z napisem
		{
			DrawRectangle(screen, SCREEN_WIDTH / 2 - DLUGOSC_BOXA/2, SCREEN_HEIGHT / 2 - 25, DLUGOSC_BOXA, 75, zielony, czarny);
			sprintf(text, "GameOver");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text, charset);
			sprintf(text, "Czy chcialbys zakonczyc gre?");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 + 10, text, charset);
			sprintf(text, "TAK(<-)  wybor strzalkami  NIE(->)");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 + 20, text, charset);
		}

		narysuj_rzad(woda1, woda1[0].ilosc_obiektow, screen, woda_1, 120, PRAWA);
		narysuj_rzad(woda2, woda2[0].ilosc_obiektow, screen, woda_2, 200, LEWA);
		narysuj_rzad(woda3, woda3[0].ilosc_obiektow, screen, woda_3, 200, LEWA);
		narysuj_rzad(woda4, woda4[0].ilosc_obiektow, screen, woda_4, 150, PRAWA);
		narysuj_rzad(woda5, woda5[0].ilosc_obiektow, screen, woda_5, 150, LEWA);
		
		///rysowanie zaby zaleznie od kierunku
		if (zycia != 0)
		{
			switch (kierunek)
			{
			case GORA:
				DrawSurface(screen, frogger, zaba.polozenie_x, zaba.polozenie_y);
				break;
			case LEWA:
				DrawSurface(screen, frogger_lewo, zaba.polozenie_x, zaba.polozenie_y);
				break;
			case PRAWA:
				DrawSurface(screen, frogger_prawo, zaba.polozenie_x, zaba.polozenie_y);
				break;
			case DOL:
				DrawSurface(screen, frogger_dol, zaba.polozenie_x, zaba.polozenie_y);
				break;
			}
		}

		///sprawdzenie czy jest na mecie---------------------
		if (meta(zaba, punkt, screen, pkt) == 0)
		{
			bonus = 50 + (10 * (CZAS -int(worldTime)));	
			czas_wyswietlania = 3;
			punktacja += bonus + 10;
			zaba.polozenie_x = SCREEN_WIDTH / 2;
			zaba.polozenie_y = START_Y;
			kierunek = GORA;
			worldTime = 0;
			for (int i = 0; i < 11; i++)czy_byl[i] = false;
		}
		else if (meta(zaba, punkt, screen, pkt) == 1)kolizja = 1;
		else if (meta(zaba, punkt, screen, pkt) == 3)
		{
			DrawRectangle(screen, SCREEN_WIDTH / 2 - DLUGOSC_BOXA/2, SCREEN_HEIGHT / 2 - 25, DLUGOSC_BOXA, 75, zielony, czarny);
			sprintf(text, "END, SCORE:  %.1li", punktacja);
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text, charset);
			sprintf(text, "Czy chcialbys zakonczyc gre?");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 + 10, text, charset);
			sprintf(text, "TAK(<-)  wybor strzalkami  NIE(->)");
			DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2 + 20, text, charset);
			zycia = 0;
		}
		///rysowanie punktacji---------------------------------------------
		sprintf(text, "SCORE: %.1li", punktacja);
		DrawString(screen, 5, SCREEN_HEIGHT - 25, text, charset);

		///wyswietlania bonusu---------------------------
		if (czas_wyswietlania > 0)
		{
			//sprintf(text, "+ %.1li", bonus);
			//DrawString(screen, zaba.polozenie_x + 20, zaba.polozenie_y - 20, text, charset);
			DrawSurface(screen, notbad,SCREEN_WIDTH/2, SCREEN_HEIGHT/2 );
		}
		//reset czasu i same rysowanie czasu-----------------------------
		if (zycia == 0)worldTime = 0;

		if(worldTime <= 40.0)DrawRectangle(screen, SCREEN_WIDTH - DLUGOSC_BOXA-1, SCREEN_HEIGHT - DLUGOSC_BOXA/10 -1, (50 - worldTime) * 6, DLUGOSC_BOXA / 10, zielony, zielony);
		else DrawRectangle(screen, SCREEN_WIDTH - DLUGOSC_BOXA - 1, SCREEN_HEIGHT - DLUGOSC_BOXA/10 -1, (50 - worldTime) * 6, DLUGOSC_BOXA / 10, czerwony,czerwony);

		sprintf(text, "TIME:");
		DrawString(screen,SCREEN_WIDTH - DLUGOSC_BOXA - 50, SCREEN_HEIGHT - 16, text, charset);
		//DrawSurface(screen, frogger, zaba.polozenie_x, zaba.polozenie_y);
		//DrawSurface(screen, eti,
		//       SCREEN_WIDTH / 2 + sin(distance) * SCREEN_HEIGHT / 3,
		//   SCREEN_HEIGHT / 2 + cos(distance) * SCREEN_HEIGHT / 3);

		///wejscie do elementu pauzy
		do {
			t2 = SDL_GetTicks();
			// w tym momencie t2-t1 to czas w milisekundach,
			// jaki uplyna³ od ostatniego narysowania ekranu
			// delta to ten sam czas w sekundach
			// here t2-t1 is the time in milliseconds since
			// the last screen was drawn
			// delta is the same time in seconds
			delta = (t2 - t1) * 0.001;
			t1 = t2;

			fpsTimer += delta;
			if (fpsTimer > 0.5) {
				fps = frames * 2;
				frames = 0;
				fpsTimer -= 0.5;
			};
			///jesli jest pauza ale nie ma wyjscia to wyswietlaj pazue------------------
			if (pauza == true && wyjscie == false)
			{
				DrawRectangle(screen, SCREEN_WIDTH / 2 - DLUGOSC_BOXA/2, SCREEN_HEIGHT / 2 - 25, DLUGOSC_BOXA, 75, zielony, czarny);
				sprintf(text, "PAUSE");
				DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text, charset);
			}
			///jesli chcemy wyjsc i mamy kilka zyc jeszcze-----------------------------
			if (wyjscie == true && zycia != 0 )
			{
				DrawRectangle(screen, SCREEN_WIDTH / 2 - DLUGOSC_BOXA/2, SCREEN_HEIGHT / 2 - 25, DLUGOSC_BOXA, 75, zielony, czarny);
				sprintf(text, "QUIT GAME?Y/N");
				DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text, charset);

			}

			// tekst informacyjny / info text
			//DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
			//"template for the second project, elapsed time = %.1lf s  %.0lf frames / s";
			//sprintf(text, "Szablon drugiego zadania, czas trwania = %.1li s  %.0lf klatek / s", worldTime, fps);
			//DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
			//	      "Esc - exit, \030 - faster, \031 - slower"
			//sprintf(text, "Esc - wyjscie, \030 - przyspieszenie, \031 - zwolnienie");
			//DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

			SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, scrtex, NULL, NULL);
			SDL_RenderPresent(renderer);

			// obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == 'q')
					{
						///tylko jesli nie mamy menu zginiecia i nie ma pazuy---------------
						if (zycia != 0 && pauza == false)
						{
							wyjscie = true;
							pauza = true;
						}
					}
					else if (event.key.keysym.sym == 'y')
					{
						if (zycia != 0 && wyjscie == true)
						{
							quit = 1;
						}
					}
					else if (event.key.keysym.sym == 'n')
					{
						if (zycia != 0 && wyjscie == true)
						{
							wyjscie = false;
							pauza = false;
						}
					}
					else if (event.key.keysym.sym == 'p')
					{
						///i jeszcze jak nie ma wyjscia to mozemy zmieniac pauze
						if (zycia != 0 && wyjscie == false)
						{
							if (pauza == false)pauza = true;
							else if (pauza == true) pauza = false;
						}
					}
					else if (event.key.keysym.sym == SDLK_UP)
					{
						//nie mozemy sie ruszac jak jest pazua lub nie mamy wyjsc
						if (zycia != 0 && pauza == false && wyjscie == false)
						{
							zaba.polozenie_y -= zaba.predkosc;
							kierunek = GORA;
						}
					}
					else if (event.key.keysym.sym == SDLK_DOWN)
					{
						if (zycia != 0 && pauza ==false && wyjscie == false)
						{
							zaba.polozenie_y += zaba.predkosc;
							kierunek = DOL;
						}
					}
					else if (event.key.keysym.sym == SDLK_RIGHT)
					{
						if (zycia != 0 && pauza == false)
						{
							zaba.polozenie_x += zaba.predkosc;
							kierunek = PRAWA;
						}
						else if(wyjscie == false)
						{
							///jak chcemy zaczac od poczatku
							zycia = 5;
							punktacja = 0;
							for (int i = 0; i < 5; i++)punkt[i].narysowany = false;
							for (int i = 0; i < 11; i++)czy_byl[i] = false;
						}
					}
					else if (event.key.keysym.sym == SDLK_LEFT)
					{
						if (zycia != 0 && pauza == false && wyjscie == false)
						{
							zaba.polozenie_x -= zaba.predkosc;
							kierunek = LEWA;
						}
						else if(zycia == 0) quit = 1;
					}
					break;
					//case SDL_KEYUP:
					//	etiSpeed = 1.0;
					//	break;
				case SDL_QUIT:
					quit = 1;
					break;
				};
			};
			if (quit == 1)break;
		} while (pauza == true);

		///sprawdzenie czy wpadla do wody---------------------------------
		if (czy_wapdla_do_wody(zaba, woda1, woda1[0].ilosc_obiektow, PRAWA) == 1) kolizja = 1;
		if (czy_wapdla_do_wody(zaba, woda2, woda2[0].ilosc_obiektow, LEWA) == 1)  kolizja = 1;
		if (czy_wapdla_do_wody(zaba, woda3, woda3[0].ilosc_obiektow, LEWA) == 1)  kolizja = 1;
		if (czy_wapdla_do_wody(zaba, woda4, woda4[0].ilosc_obiektow, PRAWA) == 1) kolizja = 1;
		if (czy_wapdla_do_wody(zaba, woda5, woda5[0].ilosc_obiektow, LEWA) == 1)  kolizja = 1;
		//umiescilem to tu na koncu ze wzgledu na kolizje i zeby jak zabka wjdze na nowe pole gdzie bedzie auto to 10 pkt sie nie doda i musi to byc po zmianie x lub y zabki
		//specjalnie po zmiany mejsca zabki
		if (sprawdz_kolizje(zaba, auto1, auto1[0].ilosc_obiektow) == 1) kolizja = 1;
		if (sprawdz_kolizje(zaba, auto2, auto2[0].ilosc_obiektow) == 1) kolizja = 1;
		if (sprawdz_kolizje(zaba, auto3, auto3[0].ilosc_obiektow) == 1) kolizja = 1;
		if (sprawdz_kolizje(zaba, auto4, auto4[0].ilosc_obiektow) == 1) kolizja = 1;
		if (sprawdz_kolizje(zaba, auto5, auto5[0].ilosc_obiektow) == 1) kolizja = 1;

		///czas normalny i czas wyswietlania bonusu
		worldTime += delta;
		czas_wyswietlania -= delta;

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		///sprawdzamy kolizje lub czy zaba wyszla poza ekran
		if (kolizja || zaba.polozenie_x >= (SCREEN_WIDTH+zaba.x/2) || zaba.polozenie_x <= 0-zaba.x/2 || zaba.polozenie_y <= 0-zaba.y/2 || zaba.polozenie_y >= SCREEN_HEIGHT+zaba.y/2 - 92 || worldTime >= CZAS)
		{
				zaba.polozenie_x = SCREEN_WIDTH / 2;
				zaba.polozenie_y = START_Y;
				kierunek = GORA;
				zycia--;
				worldTime = 0;
		}	
		///sprawdzanie punktacji czy juz zabka by³a wczesniej na polach
		for (int i = 0; i <= 5 ; i++)
		{
			if (zaba.polozenie_y - zaba.y/2 <= SCREEN_HEIGHT - (GRUBOSC_DOLNA + (i * PRZERWA)) && zaba.polozenie_y + zaba.y / 2 >= SCREEN_HEIGHT - (GRUBOSC_DOLNA + (i * PRZERWA)) && czy_byl[i]==false)
			{
				if (kolizja != 1)
				{
					czy_byl[i] = true;
					punktacja += 10;
				}
			}
		}
		for (int i = 6; i <= 11; i++)
		{
			if (zaba.polozenie_y - zaba.y / 2 <= SCREEN_HEIGHT - (GRUBOSC_GORNA + ((i-1) * PRZERWA)) && zaba.polozenie_y + zaba.y / 2 >= SCREEN_HEIGHT - (GRUBOSC_GORNA + ((i-1) * PRZERWA)) && czy_byl[i] == false)
			{
				if (kolizja != 1)
				{
					czy_byl[i] = true;
					punktacja += 10;
				}
			}
		}
		
		frames++;
		Sleep(7);
		};

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
	};
