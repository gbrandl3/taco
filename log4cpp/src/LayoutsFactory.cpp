#include <log4cpp/LayoutsFactory.hh>
#include <stdexcept>

namespace log4cpp
{
   static LayoutsFactory* layouts_factory_ = 0;

   std::auto_ptr<Layout> create_simple_layout(const FactoryParams& params);
   std::auto_ptr<Layout> create_basic_layout(const FactoryParams& params);
   std::auto_ptr<Layout> create_pattern_layout(const FactoryParams& params);
   std::auto_ptr<Layout> create_pass_through_layout(const FactoryParams& params);

   LayoutsFactory& LayoutsFactory::getInstance()
   {
      if (!layouts_factory_)
      {
         std::auto_ptr<LayoutsFactory> lf(new LayoutsFactory);
         lf->registerCreator("SimpleLayout", &create_simple_layout);
         lf->registerCreator("BasicLayout", &create_basic_layout);
         lf->registerCreator("PatternLayout", &create_pattern_layout);
         lf->registerCreator("PassThroughLayout", &create_pass_through_layout);
         layouts_factory_ = lf.release();
      }

      return *layouts_factory_;
   }

   void LayoutsFactory::registerCreator(const std::string& class_name, create_function_t create_function)
   {
      const_iterator i = creators_.find(class_name);
      if (i != creators_.end())
         throw std::invalid_argument("Layout creator for type name '" + class_name + "' already registered");

      creators_[class_name] = create_function;
   }

   std::auto_ptr<Layout> LayoutsFactory::create(const std::string& class_name, const params_t& params)
   {
      const_iterator i = creators_.find(class_name);
      if (i == creators_.end())
         throw std::invalid_argument("There is no layout with type name '" + class_name + "'");

      return (*i->second)(params);
   }

   bool LayoutsFactory::registered(const std::string& class_name) const
   {
      return creators_.find(class_name) != creators_.end();
   }
}
