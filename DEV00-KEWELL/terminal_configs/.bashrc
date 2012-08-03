# .bashrc

# User specific aliases and functions

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

# Description:  This function used to format all the source code in current forlder
#               and convert source code file format from windows to linux
#      Author:  WenJing(wenjing0101@gmail.com)
#     Version:  1.0.0 (Release by wenjing on 18th Fri. 2010)

function format_src ()
{

  find -iname "*.c" -exec dos2unix {} \;
  find -iname "*.h" -exec dos2unix {} \;
  find -iname "makefile" -exec dos2unix {} \;
  find -iname "Makefile" -exec dos2unix {} \;

  # -npro   ��Ҫ��ȡindent�������ļ�.indent.pro
  # -kr     ʹ��Kernighan&Ritchie�ĸ�ʽ
  # -i4     �������ŵĸ���Ϊ4
  # -di28   ���������εı�������ָ������λ(28) 
  # -ts4    ����tab�ĳ���Ϊ4 
  # -bls    ����ṹ��"struct"��"{"����
  # -bl     if(����else,for�ȵ�)�����ִ�����εġ�{����ͬ�У��ҡ�}���Գ�һ�С�
  # -bli0   ����{ }���ŵĸ���Ϊ0 
  # -cli2   ʹ��caseʱ��switch���ŵĸ���
  # -ss     ��for��whiile����ֻ��һ��ʱ���ڷֺ�ǰ���Ͽո�
  # -bad    ���������κ���Ͽհ���
  # -bbb    ��ע��ǰ�ӿ���
  # -bap    ����������ӿ���
  # -sc     ��ÿ��ע���������Ǻ�(*)��
  # -bc     �����������У������ֶ��ż����С�
  # -sob    ɾ������Ŀհ���
  # -l100   ��ע�����100
  # -ncs    ��Ҫ������ת������ӿո�
  # -nce    ��Ҫ��else���ڡ�}��֮��
  # -nut    ��Ҫʹ��tab������

  INDET_FORMAT="-npro -kr -i4 -ts4 -bls -bl -bli0 -cli2 -ss -bap -sc -sob -l100 -ncs -nce -nut"

  find -iname "*.c" -exec indent $INDET_FORMAT {} \;
  find -iname "*.h" -exec indent $INDET_FORMAT {} \;

  find -iname "*.h~" | xargs rm -rf {} \;
  find -iname "*.c~" | xargs rm -rf {} \;
}

# Description:  This function used to generate the NUP file
#      Author:  WenJing<wenjing0101@gmail.com>
#     Version:  1.0.0 (Release by wenjing on 24th Apr. 2011)
# 

function nup_gen ()
{
  path=`pwd`
  folder=`basename $path`
  nup_name=${folder}.nup

  #first, compress these folders and name them as ${foldername}.nup
  for i in N??*; do
       cd $i
       7za a -tzip $i.nup *
       mv $i.nup ..
       cd ..
       rm -rf $i
  done

  #second, generate the final NUP file and move it to parent folder
  7za a -tzip $nup_name *
  mv $nup_name ..
  cd ..
  rm -rf $folder
}


# Description:  This function used to uncompress the NUP file
#      Author:  WenJing<wenjing0101@gmail.com>
#     Version:  1.0.0 (Release by wenjing on 24th Apr. 2011)
# 

function nup_uncmp ()
{
  if [ $# -ne 1 ]; then
      echo "This function used to uncompress the NUP file"
      echo "Usage:   $FUNCNAME \"nup_path\"             "
      return;
  fi

  nup_path=$1
  nup_name=`basename ${nup_path}`
  work_path=`echo ${nup_name}|awk -F '.' '{print   $1}'`

  # Uncompress the TOP NUP file
  7za x $nup_path -o$work_path

  if [ ! -d  $work_path ];then
      echo "================================================================="
      echo "*  ERROR: Uncompress failure, make sure the file is zip format  *"
      echo "================================================================="
      echo ""
      return;
  fi

  # Goes into the folder and uncompress the child NUP file one by one.

  cd $work_path
  for i in *.nup; do
      fold_name=`echo $i|awk -F '.' '{print   $1}' `
      echo $fold_name
      7za x $i -o$fold_name
      rm -f $i
  done
}

