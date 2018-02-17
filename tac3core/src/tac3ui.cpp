//#include "graphics\tac3gl.h"
//#include "core\tac3ui.h"
//#include "imgui.h"
//#include <assert.h>
//#include "sdl\SDL.h"
//namespace Tac
//{
//
//  UI * gUI = nullptr;
//
//  UI::UI()
//  {
//    assert(gUI == nullptr);
//    gUI = this;
//  }
//
//  UI::~UI()
//  {
//    gUI = nullptr;
//  }
//
//  UI & UI::Instance()
//  {
//    return *gUI;
//  }
//
//
//  // This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
//  // If text or lines are blurry when integrating ImGui in your engine:
//  // - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
//  // - try adjusting ImGui::GetIO().PixelCenterOffset to 0.5f or 0.375f
//  static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count)
//  {
//    if (cmd_lists_count == 0)
//      return;
//
//    // We are using the OpenGL fixed pipeline to make the example code simpler to read!
//    // A probable faster way to render would be to collate all vertices from all cmd_lists into a single vertex buffer.
//    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers.
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glDisable(GL_CULL_FACE);
//    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_SCISSOR_TEST);
//    glEnableClientState(GL_VERTEX_ARRAY);
//    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//    glEnableClientState(GL_COLOR_ARRAY);
//
//    // Setup texture
//    glBindTexture(GL_TEXTURE_2D, fontTex);
//    glEnable(GL_TEXTURE_2D);
//
//    // Setup orthographic projection matrix
//    const float width = ImGui::GetIO().DisplaySize.x;
//    const float height = ImGui::GetIO().DisplaySize.y;
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glOrtho(0.0f, width, height, 0.0f, -1.0f, +1.0f);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//
//    // Render command lists
//    for (int n = 0; n < cmd_lists_count; n++)
//    {
//      const ImDrawList* cmd_list = cmd_lists[n];
//      const unsigned char* vtx_buffer = (const unsigned char*)cmd_list->vtx_buffer.begin();
//      glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer));
//      glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer+8));
//      glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer+16));
//
//      int vtx_offset = 0;
//      const ImDrawCmd* pcmd_end = cmd_list->commands.end();
//      for (const ImDrawCmd* pcmd = cmd_list->commands.begin(); pcmd != pcmd_end; pcmd++)
//      {
//        glScissor((int)pcmd->clip_rect.x, (int)(height - pcmd->clip_rect.w), (int)(pcmd->clip_rect.z - pcmd->clip_rect.x), (int)(pcmd->clip_rect.w - pcmd->clip_rect.y));
//        glDrawArrays(GL_TRIANGLES, vtx_offset, pcmd->vtx_count);
//        vtx_offset += pcmd->vtx_count;
//      }
//    }
//    glDisable(GL_SCISSOR_TEST);
//    glDisableClientState(GL_COLOR_ARRAY);
//    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//    glDisableClientState(GL_VERTEX_ARRAY);
//  }
//
//
//  void UI::Init(float w, float h)//SDL_Window * window)
//  {
//    //int w, h;
//    //int fb_w, fb_h;
//    //int w, h;
//    //SDL_GetWindowSize(window, &w, &h);
//
//    //glfwGetFramebufferSize(window, &fb_w, &fb_h);
//    //mousePosScale.x = (float)fb_w / w;
//    //mousePosScale.y = (float)fb_h / h;
//    mMouseScale = Vector2(1,1);
//
//    ImGuiIO& io = ImGui::GetIO();
//    io.DisplaySize = ImVec2(w,h);
//    io.DeltaTime = 1.0f/60.0f;
//    // Align OpenGL texels
//    io.PixelCenterOffset = 0.0f;
//    // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
//    
//    io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;
//    io.KeyMap[ImGuiKey_LeftArrow] = SDLK_LEFT;
//    io.KeyMap[ImGuiKey_RightArrow] = SDLK_RIGHT;
//    io.KeyMap[ImGuiKey_UpArrow] = SDLK_UP;
//    io.KeyMap[ImGuiKey_DownArrow] = SDLK_DOWN;
//    io.KeyMap[ImGuiKey_Home] = SDLK_HOME;
//    io.KeyMap[ImGuiKey_End] = SDLK_END;
//    io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
//    io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
//    io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
//    io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
//    io.KeyMap[ImGuiKey_A] = SDLK_a;
//    io.KeyMap[ImGuiKey_C] = SDLK_c;
//    io.KeyMap[ImGuiKey_V] = SDLK_v;
//    io.KeyMap[ImGuiKey_X] = SDLK_x;
//    io.KeyMap[ImGuiKey_Y] = SDLK_y;
//    io.KeyMap[ImGuiKey_Z] = SDLK_z;
//
//    io.RenderDrawListsFn = ImImpl_RenderDrawLists;
//    io.SetClipboardTextFn = ImImpl_SetClipboardTextFn;
//    io.GetClipboardTextFn = ImImpl_GetClipboardTextFn;
//#ifdef _MSC_VER
//    io.ImeSetInputScreenPosFn = ImImpl_ImeSetInputScreenPosFn;
//#endif
//
//    // Load font texture
//    glGenTextures(1, &fontTex);
//    glBindTexture(GL_TEXTURE_2D, fontTex);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//#if 1
//    // Default font (embedded in code)
//    const void* png_data;
//    unsigned int png_size;
//    ImGui::GetDefaultFontData(NULL, NULL, &png_data, &png_size);
//    int tex_x, tex_y, tex_comp;
//    void* tex_data = stbi_load_from_memory((const unsigned char*)png_data, (int)png_size, &tex_x, &tex_y, &tex_comp, 0);
//    IM_ASSERT(tex_data != NULL);
//#else
//    // Custom font from filesystem
//    io.Font = new ImBitmapFont();
//    io.Font->LoadFromFile("../../extra_fonts/mplus-2m-medium_18.fnt");
//    IM_ASSERT(io.Font->IsLoaded());
//
//    int tex_x, tex_y, tex_comp;
//    void* tex_data = stbi_load("../../extra_fonts/mplus-2m-medium_18.png", &tex_x, &tex_y, &tex_comp, 0);
//    IM_ASSERT(tex_data != NULL);
//
//    // Automatically find white pixel from the texture we just loaded
//    // (io.FontTexUvForWhite needs to contains UV coordinates pointing to a white pixel in order to render solid objects)
//    for (int tex_data_off = 0; tex_data_off < tex_x*tex_y; tex_data_off++)
//      if (((unsigned int*)tex_data)[tex_data_off] == 0xffffffff)
//      {
//        io.FontTexUvForWhite = ImVec2((float)(tex_data_off % tex_x)/(tex_x), (float)(tex_data_off / tex_x)/(tex_y));
//        break;
//      }
//#endif
//
//      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_x, tex_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
//      stbi_image_free(tex_data);
//  }
//
//
//
//}
