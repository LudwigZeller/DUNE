import styles from "../styles/Button.module.css"
import Image, {ImageProps} from "next/image";

export interface ButtonProps {
    active?: boolean,
    text: string,
    subtext?: string,
    src?: string
    onClick: () => void,
}

export default function Button({active, onClick, text, subtext, src}: ButtonProps) {
    return (
        <button className={`${styles.button} ${active && styles.active}`} onClick={onClick}>
            {src &&
                <Image width={640} height={360} src={src} alt={"Error"} className={styles.image}></Image>
            }
            <div className={styles.textbox}>
                <h1 className={styles.text}>{text}</h1>
                {subtext &&
                    <h3 className={styles.subtext}>{subtext}</h3>
                }
            </div>
        </button>
    )
}