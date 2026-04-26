'use client';

import Link from 'next/link';
import { usePathname } from 'next/navigation';
import styles from './Nav.module.css';

const LINKS = [
  { href: '/',           label: 'Atlas' },
  { href: '/about',      label: 'About' },
  { href: '/contribute', label: 'Contribute' },
];

export default function Nav() {
  const pathname = usePathname();

  return (
    <header className={styles.header}>
      <Link href="/" className={styles.logo}>
        <span className={styles.logoQ}>Q</span>
        <span className={styles.logoRest}>ATLAS</span>
      </Link>

      <nav className={styles.nav}>
        {LINKS.map(({ href, label }) => (
          <Link
            key={href}
            href={href}
            className={`${styles.navItem} ${pathname === href ? styles.active : ''}`}
          >
            {label}
          </Link>
        ))}
      </nav>

      {/* <div className={styles.headerRight}>
        <div className={styles.status}>
          <span className={styles.dot} />
          backend connected
        </div>
      </div> */}
    </header>
  );
}
