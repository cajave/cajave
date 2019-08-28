//
//      This file is automatically executed when IDA is started.
//      You can define your own IDC functions and assign hotkeys to them.
//
//      You may add your frequently used functions here and they will
//      be always available.
//
//
#include <idc.idc>
//跳转到光标处函数的开头处
static GotoCursorFuncStart()
{
   auto startaddr;
   startaddr=GetFunctionAttr(ScreenEA(),FUNCATTR_START);
   Jump(startaddr);
}
//跳转到光标处函数的结尾处
static GotoCursorFuncEnd()
{
   auto endaddr;
   endaddr=GetFunctionAttr(ScreenEA(),FUNCATTR_END);
   endaddr=FindCode(endaddr,SEARCH_UP);
   Jump(endaddr);
}

//列举指定函数的所有被调非库函数,如果本函数是叶节点非库函数，就加入全局叶函数数组中
static ListCallees(func,arridCallees,arridCnt,arridListedFlag,arridAddedFlag)
{    
      auto name,end,inst,xref,target,calleescnt,ListedCnt,bLeaf; 	
      name=Name(func);
      end=GetFunctionAttr(func,FUNCATTR_END);
      calleescnt=0;
      bLeaf=1;

      //if(name!=-1)
         //Message("------ListCallees func %s------\n",name);//死循环后会出现一直runing

      if(GetArrayElement(AR_LONG,arridListedFlag,func)==1)
          return;
      else       
          SetArrayLong(arridListedFlag,func,1);//将该函数标记为已List



      //检查第一条指令是不是jmp 导入表指令
      inst=func;
      for(target=Dfirst(inst); target!=BADADDR; target=Dnext(inst,target))//遍历第一条指令的from me数据引用列表
      {             
         xref=XrefType();
         if(xref==dr_R)
         {
            if(GetMnem(inst)=="jmp")
            {
                //检查是否是导入表中的函数
                if(!isLoaded(target))
                {
                    bLeaf=0;
                    return;
                } 
            }               
         } 
      }      

      
      for(inst=func; inst<end; inst=FindCode(inst,SEARCH_DOWN|SEARCH_NEXT))//遍历该函数的所有指令
      {        
         for(target=Rfirst(inst); target!=BADADDR; target=Rnext(inst,target))//遍历每条指令的from me代码引用列表
         {             
            xref=XrefType();     
 	    if(xref==fl_JN || xref==fl_JF) //找到了jmp跳转引用
            {

                   
                   if(GetFunctionAttr(target,FUNCATTR_START) == target && target!=func)//若jmp指令恰好跳到外部的另外一个函数
                   {
                       auto jmpfuncname;
                       jmpfuncname=Name(target);           
                       if( !(GetFunctionFlags(target) & FUNC_LIB) && strstr(jmpfuncname,"j_")!=0  && strstr(jmpfuncname,"xy")!=0 && strstr(jmpfuncname,"XY")!=0 && strstr(jmpfuncname,"Xy")!=0 )
                       {

                           bLeaf=0;
		           ListCallees(target,arridCallees,arridCnt,arridListedFlag,arridAddedFlag); 
                       }
                   }
            }                     
            else if(xref==fl_CN || xref==fl_CF) //找到了一个调用的目标函数
	    {
               auto tgtname;        
               //如果调用的不是自身，也不是库函数,也不是xy前缀开头的用户改名后的自定义函数 
               tgtname=Name(target);       
               if( target!=func && !(GetFunctionFlags(target) & FUNC_LIB) && strstr(tgtname,"j_")!=0  && strstr(tgtname,"xy")!=0 && strstr(tgtname,"XY")!=0 && strstr(tgtname,"Xy")!=0 )
               {
                   calleescnt++;
                   bLeaf=0;
                   //Message("----called %s at %d----\n",name,inst);   
                   ListCallees(target,arridCallees,arridCnt,arridListedFlag,arridAddedFlag);
               }
  
	    } 
         }
      }
      if(bLeaf)//将本函数标记为已加入
      {
         if(GetArrayElement(AR_LONG,arridAddedFlag,func)==1)
            return;
         else
         {
            auto cnt;
            //Message("%s is a leaf func-------------------------------------------\n",name);
            cnt=GetArrayElement(AR_LONG,arridCnt,0);
            SetArrayLong(arridCallees,cnt,func);
            cnt++;
            SetArrayLong(arridCnt,0,cnt);

            SetArrayLong(arridAddedFlag,func,1);//将该函数标记为已Add
         }

      }           
}
//枚举光标处函数最终调用的所有非库、未重命名的叶节点函数
static ListLeafFuns()
{
   auto func,name;
   func=GetFunctionAttr(ScreenEA(),FUNCATTR_START);
   name=Name(func);
   if(func!=-1)
   {
        auto arridCallees,arridCnt,arridListedFlag,arridAddedFlag,cnt,i,subcnt;//callees数组和全局变量元素个数
        arridCallees=GetArrayId("xy_callees");
        if(arridCallees!=-1)
        {
           DeleteArray(arridCallees);
        } 
        arridCallees=CreateArray("xy_callees");

        arridCnt=GetArrayId("xy_cnt");
        if(arridCnt!=-1)
        {
           DeleteArray(arridCnt);
        }
        arridCnt=CreateArray("xy_cnt");
        SetArrayLong(arridCnt,0,0);//cnt1=0,指除去重复后的叶节点函数个数


        arridListedFlag=GetArrayId("xy_ListedFlag");
        if(arridListedFlag!=-1)
        {
           DeleteArray(arridListedFlag);
        }
        arridListedFlag=CreateArray("xy_ListedFlag");//已List标记


        arridAddedFlag=GetArrayId("xy_AddedFlag");
        if(arridAddedFlag!=-1)
        {
           DeleteArray(arridAddedFlag);
        }
        arridAddedFlag=CreateArray("xy_AddedFlag");//已Add标记

       
	ListCallees(func,arridCallees,arridCnt,arridListedFlag,arridAddedFlag);
        cnt=GetArrayElement(AR_LONG,arridCnt,0);  

        Message("                                       %s  called  all  leaf  funcs\n",name);
        if((cnt==1 && GetArrayElement(AR_LONG,arridCallees,0)==func) || cnt==0)
        {
            Message("\n                                              oh, %s dosnot called any leaf func!\n\n",name);//自己本身就是一个叶节点函数           
        }
        else
        {        
           subcnt=0;//sub_前缀形式的函数个数
           for(i=0;i<cnt;i++)
           {
               auto funcaddr,funcname;
               funcaddr=GetArrayElement(AR_LONG,arridCallees,i);
               funcname=Name(funcaddr);                
                       if(strstr(funcname,"sub_")==0)
                       {
                          subcnt++;
                       }
                       if(funcaddr!=func)//如果不是自身
                          Message("                                                %s\n",funcname); 
                    
                           
               
           }
           Message("                                                (sum: %d,  sub_xxx: %d)\n",cnt,subcnt);
        }

        DeleteArray(arridCallees);
        DeleteArray(arridCnt);
        DeleteArray(arridListedFlag);
        DeleteArray(arridAddedFlag);
   }
   else
   {
      Warning("there is no func at current cursor"); 
   }
}





//This function is executed when IDA is started.
static main(void)
{
  auto flagline;
  flagline="-----------------";
  /////////////////////////////////////////////////热键任务/////////////////////////////////////////////////////
  if( AddHotkey("Ctrl+[","GotoCursorFuncStart")!=IDCHK_OK )
     Message("%sAddHotkey Ctrl+[ failed!(goto func start)%s\n",flagline,flagline);
  else
     Message("%sAddHotkey Ctrl+[ succeed!(goto func start)%s\n",flagline,flagline);
     
  if( AddHotkey("Ctrl+]","GotoCursorFuncEnd")!=IDCHK_OK )
     Message("%sAddHotkey Ctrl+] failed!(goto func end)%s\n",flagline,flagline);
  else
     Message("%sAddHotkey Ctrl+] succeed!(goto func end)%s\n",flagline,flagline);

  if( AddHotkey("Shift+Alt+L","ListLeafFuns")!=IDCHK_OK )
     Message("%sAddHotkey Shift+Alt+L(list all unrenamed leaf funcs) failed!%s\n",flagline,flagline);
  else
     Message("%sAddHotkey Shift+Alt+L(list all unrenamed leaf funcs) succeed!%s\n",flagline,flagline);  

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  

  // uncomment this line to remove full paths in the debugger process options:
  // SetCharPrm(INF_LFLAGS, LFLG_DBG_NOPATH|GetCharPrm(INF_LFLAGS));
}

//-----------------------------------------------------------------------
// Get name of the current processor

static get_processor_name(void)
{
  auto i,procname,chr;

  procname = "";
  for ( i=0; i < 8; i++ )
  {
    chr = GetCharPrm(INF_PROCNAME+i);
    if ( chr == 0 ) break;
    procname = procname + chr;
  }
  return procname;
}

//-----------------------------------------------------------------------
// Get signed extended 16-bit value
static SWord(ea)
{
  auto v;
  v=Word(ea);
  if ( v & 0x8000 ) v = v | 0xFFFF0000;
  return v;
}
