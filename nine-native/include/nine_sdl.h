
#ifdef __cplusplus
extern "C" {
#endif

struct SDL_Window;
struct IDirect3D9Ex* Direct3DCreate9Ex_SDL(struct SDL_Window *win);
struct IDirect3D9* Direct3DCreate9_SDL(struct SDL_Window *win);

#ifdef __cplusplus
}
#endif

