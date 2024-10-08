
#pragma once

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"  // for Button, Horizontal, Renderer
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/component_options.hpp"   // for ButtonOption
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for gauge, separator, text, vbox, operator|, Element, border
#include "ftxui/screen/color.hpp"  // for Color, Color::Blue, Color::Green, Color::Red

#include "ftxui/dom/elements.hpp"  // for Element, text, separator, window, hbox, vbox
#include "ftxui/screen/screen.hpp"  // for Screen

#include <vector>                   // for std::vector
#include <memory>

#include <cassert>


using namespace ftxui;

class OneDialogPage {
public:
    const char *name_, *text_;
    ftxui::Component btns;
    ButtonOption btns_style;
    static constexpr const char* const DefNext = "Next";
    struct NextOption{
        ButtonOption opt;
        bool optGiven = false; // trick! (see nextOption)
        const char* text = DefNext;
        bool textRightAlign;
        bool sameWidthAsOthers;
        // meaning using `{btns_style}`
        NextOption(): textRightAlign(false), sameWidthAsOthers(false){}
        NextOption(ButtonOption opt,
            const char* const text=DefNext,
            const bool textRightAlign=false,
            const bool sameWidthAsOthers=false): optGiven(true),
          opt(opt), text(text), 
          textRightAlign(textRightAlign), sameWidthAsOthers(sameWidthAsOthers){}
        NextOption(const char* text,
            const bool textRightAlign=false,
            const bool sameWidthAsOthers=false):
          text(text),
          textRightAlign(textRightAlign), sameWidthAsOthers(sameWidthAsOthers){}
        NextOption(
            const bool textRightAlign,
            const bool sameWidthAsOthers=false):
          textRightAlign(textRightAlign), sameWidthAsOthers(sameWidthAsOthers){}
    } nextOption;
    static const ButtonOption DefBtnsStyle; // see .cpp
    using Options = const std::vector<std::pair<std::string, std::function<void()>>>&;
    /*nextOptions={btns_styles}
     hint: the default value for `nextOptions` is in fact the NextButton{btns_styles}
     this just has to write as sth like this due to C++ limit.
    */
    OneDialogPage(const char* name, const char* text, Options options,
        NextOption nextOption = {},
        ButtonOption btns_style = DefBtnsStyle
    );

};

class DialogPage {
    /* this led to Segmentation fault
    using DialogPage = std::unique_ptr<DialogPage>;
    static DialogPage makeDialogPage(DialogPage& o){return std::make_unique<DialogPage>(o);}
    */
    // the same as Component,
    // just used to manage lifespan of `DialogPage` and `parent`
    class ComponentWithRC {
        using DialogPageRef = std::shared_ptr<DialogPage>;
    public:
        //static ComponentWithRC Renderer(Component children){
        //
        class Impl : public ComponentBase {

            std::function<Element()> render_;
        public:
            DialogPageRef p;
            static DialogPageRef makeRef(const DialogPage& p) {
                return std::make_shared<DialogPage>(p);
            }
            Impl(const DialogPage& p): p(makeRef(p)) {}
            //Impl(DialogPage p, bool isNew): p(p), isNew(isNew){}
            void setRender(std::function<Element()> render){
                render_ = render;
            }
            virtual ~Impl(){}
            virtual Element Render() { return render_(); }

            void addNextBtn(){
                for(auto i=0; i<p->pages.size()-1; // not the last
                        i++){
                    //p.btns->ChildAt(p.btns->ChildCount()-1)->Detach();
                    auto& cur = p->pages[i];
                    const auto& nOpt = cur.nextOption;
                    auto btn = Button(nOpt.text,
                        [this](){
                            this->p->next();
                        }, nOpt.opt
                    );
                    if (nOpt.sameWidthAsOthers) btn = btn|flex;
                    cur.btns->Add(btn);
                    //std::cerr << p->btns->ChildCount() << std::endl;

                }
            }
        };
        static std::shared_ptr<Impl> Renderer(const DialogPage& p) {
            return Make<Impl>(p);
        }

    };
    //static DialogPage makeDialogPage(DialogPage&& o){return &o;}

    //
public:
    DialogPage();
    DialogPage(const DialogPage&) = default;

    ~DialogPage();
    /* manaully call this, especially after `addNext(name, text, ...)` is called!
      NOTE: next page's live span may be longer, does this in destructor leads to Drag-pointer
     */
    DialogPage& add(OneDialogPage&& n);
    DialogPage& add(OneDialogPage& n);

    template <typename T, typename ...A>
    DialogPage& add(T name, T text, OneDialogPage::Options options, A&&... args){
        return add(
            OneDialogPage(name, text, options, std::forward<A>(args)...));
    }

    ftxui::Component asComponent(Element abovePart=filler());
    void mainloop(ScreenInteractive&);
    //DialogPage(const DialogPage& o): name_(o.name_), text_(o.text_), btns(btns), DialogPage(){}
    //const std::string& name(){return }
    const std::string curName(){return cur->name_;}

    void next(){
        //old_btns = cur->btns;
        cur++;
        idx++;
    }
    bool hasNext(){
        return cur != pages.end();
    }
private:

    //void ReleaseNext();

    //ftxui::Component old_btns;

    
    using Pages = std::vector<OneDialogPage>;
    Pages pages;
    Pages::iterator cur;
    size_t idx;
    Component getOldBtns(){return pages[idx].btns;}
    bool hasOldBtns(){return idx!=-1;}
    //#define getOldBtns() (this->pages[idx].btns)
    //ftxui::Component& parent; // used to control lifespan
    template <typename T>
    DialogPage& addImpl(DialogPage*t, T n){
        bool wasEmpty = cur==pages.end();
        pages.push_back(n);
        if(wasEmpty){ 
            //old_btns = n.btns;
            idx = -1;
            cur = pages.begin();
            return *this;
        }
        //old_btns = cur->btns;
        idx = pages.size() - 2;
        cur = pages.end()-1;
        //assert(old_btns!=nullptr);
        return *this;
    }
};