#pragma  once
#include <string>
#include <map>
namespace Tac
{
  class Renderer;
  class Scene;
  class Framebuffer;
  class Shader;
  class Camera;
  class Technique
  {
  public:
    Technique(const std::string & name);
    virtual ~Technique();
    const std::string & GetName() const;
    //virtual void Execute() = 0;
    virtual void Execute( std::map< std::string, Scene* > & scenes );
    void SetPriority(float priority);
    float GetPriority()const;
    void SetRenderer(Renderer * renderer);
    Renderer * GetRenderer();

    void SetSceneName( const std::string& sceneName );
    const std::string& GetSceneName();

    void SetFramebuffer( Framebuffer* renderTo );
    Framebuffer* GetFramebuffer();

    void SetShader( Shader* myShader );
    Shader* GetShader();

    void GetFramebufferDimensions( int* width, int* height );

    Scene* GetScene( std::map< std::string, Scene* >& scenes );
    Camera* GetCamera( Scene* myScene );
  private:
    std::string mName;
    float mPriority; // lower number is executed first
    Renderer * myRenderer;

    std::string mSceneName;

    // owned by...?
    Framebuffer* mRenderTo;

    // owned by resource manager?
    Shader* mShader;
  };
}
